/*
   Copyright © 2025 Oxford Technical Solutions (OxTS)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "ucom_decoder_app.hpp"

#include "texts.hpp"
#include "input_file.hpp"
#include "duration.hpp"
#include "cmd_line_quitter.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include <ctime>

#include "ucom/ucom_data.hpp"

#define NEWLINE '\n'

#if _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

// @brief The ucom_to_csv UCOM decoder app
UcomDecoderApp::UcomDecoderApp(int argc, char* argv[]) :
	Application(argc, argv)
{

}



// @brief Runs the UCOM decoder application
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

// @brief Processes the command-line arguments
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
            // Add error message ID
            _message_ids.push_back(UcomData::ERROR_MSG_ID);
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

        // Disable user-abort
        _disable_user_abort = _args.has_arg("-a");


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
 
// \brief Extract UCOM data from an input file
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
                if (d.get_valid())
                {
                    // Valid packet; resume search after the end of the packet
                    it += length;
                    consumed += length;
                    _packet_count++;
                    // Skip unwanted message IDs 
                    uint16_t id = d.get_message_id();
                    if (std::find(_message_ids.begin(), _message_ids.end(), id) != _message_ids.end())
                        write_csv(_output_files[id], d.get_csv());
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

// \brief Extract UCOM data from a UDP stream
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
        if (errors.size() > 0)
        {
            for (auto error : errors)
                std::cerr << error << std::endl;
        }
        return -1;
    }

    if (!_disable_user_abort)
    {
        std::cout << "Processing UDP data...";
#ifdef __linux__
        std::cout << "Enter 'q' to quit" << std::endl;
#elif _WIN32
        std::cout << "Press 'q' to quit" << std::endl;
#endif
    }

    CmdLineQuitter quitter;
    if (!_disable_user_abort)
        quitter.start();

    // Allocate a buffer to hold received UDP packets
    uint8_t buffer[4096];

    int len = 0;
    bool skip = false;
    bool packet_count_reached = false;
    bool max_capture_time_reached = false;
    Duration capture_time(_duration);
    uint64_t loop_count = 0;
    while ((len > -1) && 
        ((_max_packets == -1) || (_packet_count < _max_packets)) && 
        (_duration == -1 || !capture_time.elapsed()) && 
        !quitter.is_quit_requested())
    {
        skip = false;
        std::string source_ip;
        len = get_data(socket, buffer, 4096, source_ip);

        if (len == 0) // Timeout
        {
            std::cerr << "Timeout waiting for data...\r";
        {
            std::cerr << "Timeout waiting for data...\r";
            continue;
        }
        }

        _total_bytes += len;

        // Only process data from specified IP address
        if (( source_ip.compare(_filter_ip) == 0) || (_filter_ip.compare("any") == 0) || _filter_ip.empty())
        {
            // Create a UcomData instance from the received data
            UcomData data{ buffer, len, _dbu };

            // Skip any invalid packets
            if (!data.get_valid())
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

            // Check if it's an error message
            if (data.get_error_no() != 0)
            {
                std::cerr << "Error number: " << std::to_string(data.get_error_no()) << ", Error messages : " << data.get_error_messages() << std::endl;
                write_csv(_output_files[data.get_message_id()], data.get_csv());
            }
            else if (!skip && data.get_valid()) {
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

        loop_count++;
        if (loop_count % 50 == 0)
            std::cout << "Bytes processed: " << _total_bytes << "\r";
        loop_count++;
        if (loop_count % 50 == 0)
            std::cout << "Bytes processed: " << _total_bytes << "\r";
    }

    std::cout << '\n';
    close_output_files();

    if (!_disable_user_abort && !quitter.is_quit_requested())
    {
        std::cout << "Data collection complete. ";
#ifdef __linux__
        std::cout << "Enter 'f' to finish" << std::endl;
#elif _WIN32
        std::cout << "Press 'f' to finish" << std::endl;
#endif
    }

    return 0;
}

// \brief Gets data from a UDP socket
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

// @brief Creates the time-stamped output folder
bool UcomDecoderApp::create_output_dir(std::string &dir_name)
{
    auto now = std::chrono::system_clock::now();
    time_t tm = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&tm), "%y%m%d-%H%M%S");
    dir_name = ss.str();
    return std::filesystem::create_directory(dir_name);
}

// @brief Creates the output files
bool UcomDecoderApp::create_output_files()
{
    std::string dir_name;
    if (!create_output_dir(dir_name))
        return false;

    std::string path = dir_name.append(PATH_SEPARATOR).append(_output_file_prefix);
    for (auto id : _message_ids)
    {
        _output_files.insert({ id, std::fstream() });
        if (!create_output_file(path, id, _dbu.get_message(id).get_header(), _output_files[id]))
        if (!create_output_file(path, id, _dbu.get_message(id).get_header(), _output_files[id]))
            return false;
    }
    return true;
}

// @brief Closes the output files
void UcomDecoderApp::close_output_files()
{
    for (std::map<int, std::fstream>::iterator it = _output_files.begin(); it != _output_files.end(); it++)
    {
        if (it->second.is_open())
            it->second.close();
    }
}

// @brief Creates an output file
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

// @brief Writes a line of CSV output to file and terminates with a new-line
void UcomDecoderApp::write_csv(std::fstream& output_stream, const std::string &csv)
{
    output_stream << csv << '\n';
}

// @brief Outputs the help text to std out
void UcomDecoderApp::print_help_text()
{
    for (auto line : help_text)
        std::cout << line << '\n';
    std::cout << std::flush;
}

// @brief Outputs the banner text to std out
void UcomDecoderApp::print_banner_text()
{
    for (auto line : banner_text)
        std::cout << line << '\n';
    std::cout << std::flush;
}

// @brief Outputs the decoder statistics to std out
void UcomDecoderApp::display_statistics()
{
    std::cout << "Total bytes read: " << _total_bytes << NEWLINE;
    std::cout << "Valid packets decoded: " << _packet_count << NEWLINE;
    std::cout << "Skipped packets: " << _skipped_packets << NEWLINE;
    if (!_process_file)
        std::cout << "Filtered packets: " << _filtered_packets << NEWLINE;
    std::cout << "Invalid packets: " << _invalid_packets << std::endl;
    

}
