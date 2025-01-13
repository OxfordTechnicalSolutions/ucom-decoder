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
    T get_data(const uint8_t* data, int offset) {
        T *ptr = (T*)&data[offset];
        return *ptr;
    }

    template <typename T>
    T get_data_update_offset(const uint8_t* data, int &offset) {
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
            }
            _values.push_back(value);
        }
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


int main(int argc, char* argv[])
{
    std::cout << "UCOM decoder" << std::endl;
    Args args{argc, argv};

    // The number of packets to capture (zero means capture indefinitely)
    int max_packets = -1;

    std::map<int, UcomMessage> ucom;
    std::map<int, std::vector<std::string>> all_data;


    
    std::string dbu;
    if (args.size() > 0) {
        for (auto arg : args)
            std::cout << arg.first << " : " << arg.second << std::endl;
        
        if (args.get_arg(std::string("-u"), dbu))
        {
            std::cout << "Using .dbu file: " << dbu << std::endl;

            std::ifstream f(dbu);
            if (!f.is_open())
            {
                std::cerr << "Unable to open file: " << dbu << std::endl;

                return -1;
            }
        }

        // Retrieve x packets
        std::string packets;
        if (args.get_arg("-c", packets))
        {
            max_packets = std::atoi(packets.c_str());
            std::cout << "Capturing " << max_packets << "packets";
        }
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

    // Get data only from this IP address
    const std::string filter_ip("195.0.0.84");

    // Get data only from these message IDs
    const std::list<uint16_t>& message_ids = { 0,1,2,3 };
    
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



