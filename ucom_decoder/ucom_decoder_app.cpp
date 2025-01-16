#include "ucom_decoder_app.hpp"
#include "input_file.hpp"
#include "ucom_data.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

UcomDecoderApp::UcomDecoderApp(int argc, char* argv[]) :
	Application(argc, argv)
{

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

    int pkt_count = 0;
    int total_bytes = 0;
    int skipped_pkts = 0;
    int consumed = 0;
    int available = 0;
    int offset = 0;
    int it_offset = 0;
    bool need_more_data = false;
    std::vector<char> data;
    while (left > 0)
    {
        offset = consumed - available;
        data = input.get_data(data_available, left, offset);
        consumed = 0;
        available = data.size();
        for (auto it = data.begin(); it < data.end();)
        {
            need_more_data = false;
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
                    pkt_count++;
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
        total_bytes += consumed;

    }

    std::cout << "Read " << pkt_count << " packets" << std::endl;
    std::cout << "Total bytes read " << total_bytes << std::endl;

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

    // Allocate a buffer to hold received UDP packets
    uint8_t buffer[4096];

    int skipped_packets = 0;
    int packet_count = 0;
    int len = 0;
    while ((len > -1) && ((_max_packets == -1) || (packet_count < _max_packets)))
    {
        std::string source_ip;
        len = get_data(socket, buffer, 4096, source_ip);
        if (source_ip.compare(_filter_ip) != 0)
        {
            std::cout << "Filter IP mismatch: " << source_ip << std::endl;
            continue;
        }

        // Create a UcomData instance from the received data
        UcomData data{ buffer, len, _dbu };

        if (!data.IsValid())
        {
            std::cout << "Data packet is invalid, skipping" << std::endl;
            skipped_packets++;
            continue;
        }

        if (std::find(_message_ids.begin(), _message_ids.end(), data.get_message_id()) == _message_ids.end())
        {
            std::cout << "Skipping message ID: " << data.get_message_id() << std::endl;
            continue;
        }

        if (data.IsValid()) {
            std::cout << "Message: " << " ID: " << data.get_message_id()
                << " Version: " << data.get_message_version()
                << " Time Frame: " << (uint16_t)data.get_time_frame()
                << " Arb. Time: " << data.get_arbitrary_time()
                << " Payload length: " << data.get_payload_length()
                << " Signal count: " << data.get_signal_count()
                << " CRC (calc.): " << data.get_calc_crc() << std::endl;

            // Write the message data to output
            if (_dbu.message_id_exists(data.get_message_id()))
            {
                write_csv(_output_files[data.get_message_id()], data.get_csv());
                _max_packets--;

            }

        }
    }

    close_output_files();

    std::cout << "Skipped packets: " << skipped_packets << std::endl;
    return 0;
}

int UcomDecoderApp::run() {
    std::cout << "UCOM decoder" << std::endl;

    // Process the command-line arguments
    if (_args.size() > 0) {
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
                std::cout << "Processing file: " << _input_filename << std::endl;
                f.close();
            }
        }

        // Retrieve x packets
        std::string packets;
        if (_args.get_arg("-c", packets))
        {
            _max_packets = std::atoi(packets.c_str());
            std::cout << "Capturing " << _max_packets << " packets" << std::endl;
        }

        // Source IP filtering
        if (_args.get_arg("-i", _filter_ip))
        {
            std::cout << "Source IP filter: " << _filter_ip << std::endl;
        }

        std::string ids;
        // Message IDs
        if (_args.get_arg("-m", ids))
        {
            std::cout << "Message IDs:";

            std::string id;
            std::istringstream is(ids);
            while (std::getline(is, id, ' '))
            {
                try {
                    int value = std::stoi(id);
                    std::cout << " " << value;
                    _message_ids.push_back(value);
                }
                catch (...) {
                    // Failed to convert string to int
                    std::cout << std::endl << "Error parsing message ID: " << id << std::endl;
                    return -1;
                }
            }
            std::cout << std::endl;
        }
        else
        {
            // Use all message ids
            _message_ids = _dbu.get_message_ids();
        }
    }
    else
    {
        std::cout << "Failed to process command-line arguments" << std::endl;
        return -1;
    }

    if (_process_file)
    {
        return process_file();
    }
    else
    {
        return process_udp();
    }

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
        if (!create_output_file("output_", id, _dbu.get_message(id).get_header(), _output_files[id]))
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
