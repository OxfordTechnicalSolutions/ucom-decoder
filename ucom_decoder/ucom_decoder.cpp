#include <iostream>
#include <algorithm> // for find_if
#include <map>
#include "string_helpers.hpp" // for trim, ltrim, rtrim
#include "args.hpp"
#include "nlohmann/json.hpp" // for JSON parser
#include <fstream> // for ifstream
#include "ucom_message.hpp"
#include "crc.hpp"
#include "ucom_dbu.hpp"
#include "socket_helper.hpp"
#include <list>

//using json = nlohmann::json;

#define PORT 50487

int get_data(Socket &socket, uint8_t* buffer, int max_len, std::string &source_ip)
{
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int error = 0;
    uint32_t in_ip;
    int recvLen = socket.recv((char *)buffer, max_len, source_ip, in_ip, error);
    if (recvLen < 0) {
        std::cerr << "Error receiving data" << std::endl;
        return -1;
    }

    buffer[recvLen] = '\0'; // Null-terminate the received data
    std::cout << "Received packet: " << buffer << std::endl;

    return recvLen;
}

class UCOMData {
    private:
    uint16_t _message_id; // Bytes 2-3
    uint8_t _message_version; // Byte 4
    uint8_t _time_frame; // Byte 5
    int64_t _arbitrary_time; // Bytes 6-13
    uint16_t _payload_length; // Bytes 14-15
    int64_t _gnsst; // Bytes 16-23
    uint32_t _calc_crc; // Bytes 16 + _payload_length 
    bool _valid;
    std::vector<double> _values; // collection for signal values
    double get_double(uint8_t* data, int offset) {
        double *ptr = (double*)&data[offset];
        return *ptr;
    }
    template <typename T>
    static T get_data(const uint8_t* data, int offset) {
        T *ptr = (T*)&data[offset];
        return *ptr;
    }

    template <typename T>
    static T get_data_update_offset(const uint8_t* data, int &offset) {
        T *ptr = (T*)&data[offset];
        offset += sizeof(T);
        return *ptr;
    }
    public:
    UCOMData(const uint8_t* data, int size, UcomDbu& dbu)
    {
        _valid = false;
        // Check that we at least have a header
        if (size < 16)
            return;

        // Check the sync bytes are correct
        if ((data[0] != 'U') || (data[1] != 'M'))
            return;

        // Extract data from header
        _message_id = get_data<uint16_t>(data, 2);
        _message_version = data[4];
        _time_frame = data[5];
        _arbitrary_time = get_data<int64_t>(data, 6);
        _payload_length = get_data<uint16_t>(data, 14);



        // Check that we have a full packet
        if ((_payload_length + 20) > size)
            return;

        // Check the CRC
        uint32_t crc = get_data<uint32_t>(data, _payload_length + 16);
        _calc_crc = crc32(data, _payload_length + 16);
        if (crc != _calc_crc)
            return;

        _valid = true;
        
        int i = 16;
        // Step through the signals and decode according to their type
        _values.clear();
        for (auto signal : dbu.get_signals(_message_id))
        {
            double value;
            OxTS::Enum::BASIC_TYPE type = signal->get_data_type();
            switch (type)
            {
                case OxTS::Enum::BASIC_TYPE::BASIC_TYPE_int64_t:
                    value = get_data_update_offset<int64_t>(data, i);
                    break;
                case OxTS::Enum::BASIC_TYPE::BASIC_TYPE_double:
                    value = get_data_update_offset<double>(data, i);
                    break;
                default:
                    value = nan("");
            }
            _values.push_back(value);
        }
    }

    static const int peek(const uint8_t* data, int max_size)
    {
        bool valid = false;
        // Check that we at least have a header
        if (max_size < 16)
            return -1;

        // Check the sync bytes are correct
        if ((data[0] != 'U') || (data[1] != 'M'))
            return -1;

        // Extract data from header
        uint16_t payload_length = get_data<uint16_t>(data, 14);

        // Check that we have a full packet
        if ((payload_length + 20) > max_size)
            return -1;

        return payload_length + 20;
    }

    uint16_t get_message_id() {
        return _message_id;
    }

    uint16_t get_message_version() {
        return _message_version;
    }

    uint8_t get_time_frame() {
        return _time_frame;
    }

    int64_t get_arbitrary_time() {
        return _arbitrary_time;
    }

    uint16_t get_payload_length() {
        return _payload_length;
    }

    int get_signal_count()
    {
        // The count of all the doubles, plus arbitrary time and GNSST
        return _values.size() + 2;
    }

    uint32_t get_calc_crc()
    {
        return _calc_crc;
    }

    bool IsValid() {
        return _valid;
    }

    std::string get_csv()
    {
        std::stringstream ss;
        ss.precision(7);
        ss << _arbitrary_time;
        for (double value : _values)
            ss << "," << std::fixed << value;
        return ss.str();
    }

    std::string to_string()
    {
        std::stringstream ss;
        ss << "ID: " << get_message_id() << " Version: " << get_message_version() << " Payload length: " << get_payload_length();
        return ss.str();
    }

};

std::string create_header(json message)
{
    std::stringstream ss;
    ss << "Time (" << message["MessageTiming"].get<std::string>() << ")";
    for (auto msg : message["SignalsInMessage"])
        ss << "," << msg["SignalID"].get<std::string>();
    return ss.str();
}

void write_csv(std::map<int, std::vector<std::string>> all_data, std::map<int, json> messages)
{
    for (auto data : all_data)
    {
        std::stringstream filename;
        filename << "output_" << std::setfill('0') << std::setw(3) << data.first << ".csv"; 
        std::cout << "Creating output file: " << filename.str() << std::endl;
        std::fstream f(filename.str(), std::ios_base::trunc | std::ios_base::out);
        if (!f.is_open())
            std::cout << "Failed to open file" << std::endl;
        f << create_header(messages[data.first]) << std::endl;
        for (std::string line : data.second)
            f << line << '\n';
        f.close();
    }
}

void write_csv(const std::map<int, std::vector<std::string>> &all_data, std::map<uint16_t, UcomMessage> &messages)
{
    for (auto data : all_data)
    {
        std::stringstream filename;
        filename << "output_" << std::setfill('0') << std::setw(3) << data.first << ".csv"; 
        std::cout << "Creating output file: " << filename.str() << std::endl;
        std::fstream f(filename.str(), std::ios_base::trunc | std::ios_base::out);
        if (!f.is_open())
            std::cout << "Failed to open file" << std::endl;
        if (messages.find(data.first) != messages.end())
            f << (messages.find(data.first))->second.get_header() << std::endl;
        for (std::string line : data.second)
            f << line << '\n';
        f.close();
    }
}

void write_csv(std::map<int, std::vector<std::string>> &all_data, UcomDbu &dbu)
{
    write_csv(all_data, dbu.get_messages());
}

bool get_file_data(std::string filename, std::vector<char>& data, int64_t &left)
{
    std::ifstream f(filename, std::ios::binary);
    f.seekg(0, f.end);
    std::streampos size = f.tellg();
    f.seekg(0, f.beg);
    if (f.good())
    {
        std::cout << "Size before file read: " << data.size() << '\n';
        std::cout << "Vector capacity: " << data.capacity() << '\n';
        data.resize(data.capacity());
        f.read(& data[0], data.capacity());
        int read = f.gcount();
        
        if (f.eof())
        {
            // End-of-file
            left = 0;
        }
        else
            left = size - f.tellg();

        // Resize vector to match data read
        data.resize(read);

        std::cout << "Size after file read: " << data.size() << std::endl;
        return true;
    }
    else
        return false;
}


int main(int argc, char* argv[])
{
    std::cout << "UCOM decoder" << std::endl;
    Args args{argc, argv};

    /* Command-line arguments
    */

    // .dbu filename
    std::string dbu;

    // The number of packets to capture (zero means capture indefinitely)
    int max_packets = -1;

    // Get data only from this IP address
    std::string filter_ip;

    // Get data only from these message IDs
    std::list<uint16_t> message_ids;

    // Process a file rather than UDP stream (default)
    // -f <filename>
    std::string filename;
    bool process_file = false;

    std::map<int, UcomMessage> ucom;
    std::map<int, std::vector<std::string>> all_data;

    if (args.size() > 0) {
        for (auto arg : args)
            std::cout << arg.first << " : " << arg.second << std::endl;
        
        if (args.get_arg("-u", dbu))
        {
            std::ifstream f(dbu);
            if (!f.is_open())
            {
                std::cerr << "Unable to open file: " << dbu << std::endl;
                return -1;
            }
            else
            {
                std::cout << "Using .dbu file: " << dbu << std::endl;
                f.close();
            }
        }
        else
        {
            std::cout << "Please specify .dbu file to use [ -u <filename> ]" << std::endl;
            return -1;
        }

        if (args.get_arg("-f", filename))
        {
            std::fstream f(filename);
            if (!f.is_open())
            {
                std::cerr << "Unable to open file: " << filename << std::endl;
                return -1;
            }
            else
            {
                process_file = true;
                std::cout << "Processing file: " << filename << std::endl;
                f.close();

                
            }
        }
        UcomDbu a_dbu(dbu);
        if (process_file)
        {
            std::vector<char> data;
            data.reserve(8388608);
            int64_t left = 0;
            get_file_data(filename, data, left);
            int pkt_count = 0;

            std::stringstream output_filename;
            output_filename << "output_" << std::setfill('0') << std::setw(3) << "file" << ".csv";
            std::cout << "Creating output file: " << output_filename.str() << std::endl;
            std::fstream f(output_filename.str(), std::ios_base::trunc | std::ios_base::out);
            if (!f.is_open())
                std::cout << "Failed to open file" << std::endl;


            for (auto it = data.begin(); it <= data.end();)
            {
                int length = UCOMData::peek(reinterpret_cast<uint8_t*>(&(*it)), data.end() - it);
                if (length >= 20)
                {
                    // Found a candidate for a valid packet - try to extract the data
                    UCOMData d(reinterpret_cast<uint8_t*>(&(*it)), length, a_dbu);
                    if (d.IsValid())
                    {
                        it += length;
                        pkt_count++;
                        f << d.get_csv() << '\n';
                        //std::cout << d.to_string() << ", Data left: " << data.end() - it <<  std::endl;
                    }
                    else
                        it++;
                }
                else
                {
                    it++;
                }
            }

            std::cout << "Read " << pkt_count << " packets" << std::endl;
            f.close();
        }



        // Retrieve x packets
        std::string packets;
        if (args.get_arg("-c", packets))
        {
            max_packets = std::atoi(packets.c_str());
            std::cout << "Capturing " << max_packets << " packets" << std::endl;
        }

        // Source IP filtering
        if (args.get_arg("-i", filter_ip))
        {   
            std::cout << "Source IP filter: " << filter_ip << std::endl;
        }

        std::string ids;
        // Message IDs
        if (args.get_arg("-m", ids))
        {
            std::cout << "Message IDs:";
            
            std::string id;
            std::istringstream is(ids);
            while (std::getline(is, id, ' '))
            {
                try {
                    int value = std::stoi(id);
                    std::cout << " " << value;
                    message_ids.push_back(value);
                }
                catch (...) {
                    // Failed to convert string to int
                    std::cout << std::endl << "Error parsing message ID: " << id << std::endl;
                    return -1;
                }
            }
            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to process command-line arguments" << std::endl;
        return -1;
    }


    // Extract message / signal info from the .dbu file
    UcomDbu the_dbu(dbu);
    if (!the_dbu.get_valid())
        std::cout << "Failed to parse " << dbu << std::endl;

    std::vector<std::string> errors;
    Socket socket("0.0.0.0", 50487, "127.0.0.1", 50487, errors);

    if (!socket.is_initialised())
        return -1;

    uint8_t buffer[4096];
    
    int skipped_packets = 0;
    int packet_count = 0;
    int len = 0;
    while((len > -1) && ((max_packets == -1) || (packet_count < max_packets)))
    {
        std::string source_ip;
        len = get_data(socket, buffer, 4096, source_ip);
        if (source_ip.compare(filter_ip) != 0)
        {
            std::cout << "Filter IP mismatch: " << source_ip << std::endl;
            continue;
        }

        // Create a UCOMData instance from the received data
        UCOMData data{buffer, len, the_dbu};

        if (!data.IsValid())
        {
            std::cout << "Data packet is invalid, skipping" << std::endl;
            skipped_packets++;
            continue;
        }

        if (std::find(message_ids.begin(), message_ids.end(), data.get_message_id()) == message_ids.end())
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
            
            // Add the message data to the collection of messages
            if (the_dbu.message_id_exists(data.get_message_id()))
            {
                if (!all_data.contains(data.get_message_id()))
                    all_data.insert(std::make_pair(data.get_message_id(), std::vector<std::string>()));
                
                // Store the UCOM data (csv format) in the relevant location
                all_data[data.get_message_id()].push_back(data.get_csv());
                max_packets--;
                
            }
            std::cout << data.get_csv() << std::endl;

            
        }
        else
        {
            std::cout << "Message: Invalid" << std::endl;
            len = -1;
        }
    }

    write_csv(all_data, the_dbu);
    std::cout << "Skipped packets: " << skipped_packets << std::endl;

}



