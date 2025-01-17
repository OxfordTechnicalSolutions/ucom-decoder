#include "ucom_decoder_app.hpp"
#include "input_file.hpp"
#include "ucom_data.hpp"
#include "texts.hpp"
#include "duration.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>


#define NEWLINE '\n'

UcomDecoderApp::UcomDecoderApp(int argc, char* argv[]) :
	Application(argc, argv)
{

}



//! @brief Runs the UCOM decoder application
int UcomDecoderApp::run() {
    // Process command-line arguments
    int result = process_args();
    if (result != 0)
        return result;
    
    if (_process_file)
    {
        // Extract UCOM data from a file
        result = process_file();
    }
    else
    {
        // Extract UCOM data from a UDP stream
        result = process_udp();
    }

    display_statistics();

    return result;
}

int UcomDecoderApp::process_args()
{
    // Process the command-line arguments
    if (_args.size() > 0) {
        if (_args.has_arg("-h"))
        {
            print_help_text();
            return -1;
        }

        print_banner_text();

        if (_args.get_arg("-u", _dbu_filename))
        {
            _dbu = UcomDbu(_dbu_filename);

            if (!_dbu.get_valid())
            {
                std::cerr << "Failed to read .dbu data from: " << _dbu_filename << std::endl;
                return -1;
            }
            else
            {
                std::cout << "Using .dbu file: " << _dbu_filename << std::endl;
            }
        }
        else
        {
            std::cout << "Please specify .dbu file to use [ -u <filename> ]" << std::endl;
            return -1;
        }

        /* Option: -f
        *  Description: Extract UCOM data from an input file
        *  Value: Path to input file
        *  Usage: -f <input file>
        */
        if (_args.get_arg("-f", _input_filename))
        {
            std::fstream f(_input_filename);
            if (!f.is_open())
            {
                std::cerr << "Unable to open file: " << _input_filename << std::endl;
                return -1;
            }
            else
            {
                _process_file = true;
                std::cout << "Processing input file: " << _input_filename << std::endl;
                f.close();
            }
        }

        // Retrieve x packets
        std::string packets;
        if (_args.get_arg("-c", packets))
        {
            _max_packets = std::atoi(packets.c_str());
            if (!_process_file)
                std::cout << "Capturing " << _max_packets << " packets (max)" << std::endl;
        }

        // Source IP filtering
        if (_args.get_arg("-i", _filter_ip))
        {
            if (!_process_file)
                std::cout << "Source IP filter: " << _filter_ip << std::endl;
        }

        std::string ids;
        // Message IDs
        if (_args.get_arg("-m", ids))
        {
            std::string id;
            std::istringstream is(ids);
            while (std::getline(is, id, ' '))
            {
                try {
                    int value = std::stoi(id);
                    _message_ids.push_back(value);
                }
                catch (...) {
                    // Failed to convert string to int
                    std::cerr << std::endl << "Error parsing message ID: " << id << std::endl;
                    return -1;
                }
            }
        }
        else
        {
            // Use all message ids
            _message_ids = _dbu.get_message_ids();
        }

        // Output file prefix
        _args.get_arg("-o", _output_file_prefix);

        // Capture duration
        std::string duration;
        if (_args.get_arg("-t", duration))
        {
            try {
                _duration = std::stoi(duration);
            }
            catch (...) {
                // Failed to convert string to int
                std::cerr << std::endl << "Error parsing capture duration (-t): " << duration << std::endl;
                return -1;
            }
            if (!_process_file)
                std::cout << "Maximum capture duration: " << _duration << " seconds" << std::endl;
        }

        std::cout << "Processing message IDs:";
        for (auto id : _message_ids)
            std::cout << " " << id;
        std::cout << std::endl;
    }
    else
    {
        print_help_text();
        return -1;
    }
    return 0;
}

//! 
//! \brief Extract UCOM data from an input file
int UcomDecoderApp::process_file()
{
    // Create the output files; abort if creation fails
    if (!create_output_files())
        return -1;

    InputFile input(_input_filename);
    bool data_available;
    int64_t left = input.get_file_size();

    int consumed = 0;
    int available = 0;
    int offset = 0;
    int it_offset = 0;
    bool need_more_data = false;
    std::vector<char> data;
    // Repeat until there is no unread data left
    while (left > 0)
    {
        offset = consumed - available;
        // Get data from the input file
        data = input.get_data(data_available, left, offset);
        consumed = 0;
        available = data.size();
        for (auto it = data.begin(); it < data.end();)
        {
            need_more_data = false;
            // Check if the data contains a candidate UCOM packet
            int length = UcomData::peek(reinterpret_cast<uint8_t*>(&(*it)), data.end() - it, need_more_data);
            if (length >= 20)
            {
                // Found a candidate for a valid packet - try to extract the data
                UcomData d(reinterpret_cast<uint8_t*>(&(*it)), length, _dbu);
                if (d.IsValid())
                {
                    // Valid packet; resume search after the end of the packet
                    it += length;
                    consumed += length;
                    _packet_count++;
                    write_csv(_output_files[d.get_message_id()], d.get_csv());
                }
                else
                {
                    // Not a valid packet; step to next byte
                    it++;
                    if (it <= data.end())
                        consumed++;
                }
            }
            else
            {
                if (need_more_data)
                {
                    // Found the start of a possible candidate, but need more data
                    it_offset = data.end() - it;
                    break;
                }
                it++;
                if (it <= data.end())
                    consumed++;
            }
            it_offset = data.end() - it;
        }
        _total_bytes += consumed;

    }

    close_output_files();

    return 0;
}

//!
//! \brief Extract UCOM data from a UDP stream
int UcomDecoderApp::process_udp()
{
    // Create the output files; abort if creation fails
    if (!create_output_files())
        return -1;

    // Create a socket instance; abort if creation fails
    std::vector<std::string> errors;
    Socket socket("0.0.0.0", DEFAULT_PORT, "127.0.0.1", 50487, errors);

    if (!socket.is_initialised())
    {
        std::cerr << "Failed to initialise socket" << std::endl;
        return -1;
    }

    std::cout << "Waiting for data..." << std::endl;

    // Allocate a buffer to hold received UDP packets
    uint8_t buffer[4096];

    int len = 0;
    bool skip = false;
    Duration capture_time(_duration);
    while ((len > -1) && (_packet_count < _max_packets) && (_duration > -1 && !capture_time.elapsed()))
    {
        skip = false;
        std::string source_ip;
        len = get_data(socket, buffer, 4096, source_ip);
        _total_bytes += len;

        // Only process data from specified IP address
        if (source_ip.compare(_filter_ip) == 0)
        {
            // Create a UcomData instance from the received data
            UcomData data{ buffer, len, _dbu };

            // Skip any invalid packets
            if (!data.IsValid())
            {
                skip = true;
                _invalid_packets++;
            }

            // Skip unwanted message IDs 
            uint16_t id = data.get_message_id();
            if (std::find(_message_ids.begin(), _message_ids.end(), id) == _message_ids.end())
            {
                skip = true;
                _skipped_packets++;
                if (_skipped_ids.find(id) == _skipped_ids.end())
                {
                    std::cout << "Skipping messages with ID: " << id << std::endl;
                    _skipped_ids.insert(id);
                }
            }

            if (!skip && data.IsValid()) {
                // Write the message data to output
                if (_dbu.message_id_exists(data.get_message_id()))
                {
                    write_csv(_output_files[data.get_message_id()], data.get_csv());
                    _packet_count++;
                }
            }
        }
        else
        {
            if (_filtered_ips.find(source_ip) == _filtered_ips.end())
            {
                std::cout << "Ignoring packets from: " << source_ip << std::endl;
                _filtered_ips.insert(source_ip);
            }
            _filtered_packets++;
        }
    }

    close_output_files();

    return 0;
}

//! 
//! \brief Gets data from a UDP socket
int UcomDecoderApp::get_data(Socket& socket, uint8_t* buffer, int max_len, std::string& source_ip)
{
    int error = 0;
    uint32_t in_ip;
    int recvLen = socket.recv((char*)buffer, max_len, source_ip, in_ip, error);
    if (recvLen < 0) {
        std::cerr << "Error receiving data: " << error << std::endl;
        return -1;
    }

    return recvLen;
}

bool UcomDecoderApp::create_output_files()
{
    for (auto id : _message_ids)
    {
        _output_files.insert({ id, std::fstream() });
        if (!create_output_file(_output_file_prefix, id, _dbu.get_message(id).get_header(), _output_files[id]))
            return false;
    }
    return true;
}

void UcomDecoderApp::close_output_files()
{
    for (std::map<int, std::fstream>::iterator it = _output_files.begin(); it != _output_files.end(); it++)
    {
        if (it->second.is_open())
            it->second.close();
    }
}

bool UcomDecoderApp::create_output_file(const std::string& filename, int message_id, const std::string &header, std::fstream &output_stream)
{
    std::stringstream ssfilename;
    ssfilename << filename << std::setfill('0') << std::setw(3) << message_id << ".csv";
    std::cout << "Creating output file: " << ssfilename.str() << std::endl;
    output_stream.open(ssfilename.str(), std::ios_base::trunc | std::ios_base::out);
    if (!output_stream.is_open())
    {
        std::cerr << "Failed to create output file: " << ssfilename.str() << std::endl;
        return false;
    }
    output_stream << header << '\n';
    return true;
}


void UcomDecoderApp::write_csv(std::fstream& output_stream, const std::string &csv)
{
    output_stream << csv << '\n';
}

void UcomDecoderApp::print_help_text()
{
    for (auto line : help_text)
        std::cout << line << '\n';
    std::cout << std::flush;
}

void UcomDecoderApp::print_banner_text()
{
    for (auto line : banner_text)
        std::cout << line << '\n';
    std::cout << std::flush;
}

void UcomDecoderApp::display_statistics()
{
    std::cout << "Total bytes read: " << _total_bytes << NEWLINE;
    std::cout << "Valid packets decoded: " << _packet_count << NEWLINE;
    std::cout << "Skipped packets: " << _skipped_packets << NEWLINE;
    if (!_process_file)
        std::cout << "Filtered packets: " << _filtered_packets << NEWLINE;
    std::cout << "Invalid packets: " << _invalid_packets << std::endl;
    

}
