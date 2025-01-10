#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm> // for find_if
#include <map>
#include "string_helpers.hpp" // for trim, ltrim, rtrim
#include "args.hpp"
#include "nlohmann/json.hpp" // for JSON parser
#include <fstream> // for ifstream
#include "ucom_message.hpp"
#include "crc.hpp"
#include "ucom_dbs.hpp"
#include "ucom_dbu.hpp"

using json = nlohmann::json;

#define PORT 50487

int create_socket()
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cout << "socket() => -1, errno=" << errno << std::endl;
        return -1;
    }

    int reuseaddr = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) {
        std::cout << "setsockopt() => -1, errno=" << errno << std::endl;
        return -1;
    }

    struct sockaddr_in servaddr = {0, 0, 0, 0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // binding to socket that will listen for new connections
    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        std::cout << "bind() => -1, errno=" << errno << std::endl;
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int get_data(int sockfd, uint8_t* buffer, int max_len)
{
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    int recvLen = recvfrom(sockfd, buffer, max_len, 0, (struct sockaddr*)&clientAddr, &addrLen);
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
        _calc_crc = crc32(&data[16], _payload_length);
        if (crc != _calc_crc)
            return;

        _valid = true;

        // Loop through the payload, 8 bytes at a time
        for (int i = 16; i < _payload_length + 16; i += 8)
        {
            // First signal is int64_t ...
            if (i == 16)
                _gnsst = get_data<int64_t>(data, i);
            else    // ... the rest are doubles
                _values.push_back(get_data<double>(data, i));
        }
        
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
        ss << "," << _gnsst;
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

void write_csv(std::map<int, std::vector<std::string>> all_data, std::map<int, UcomMessage> messages)
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
            f << messages[data.first].get_header() << std::endl;
        for (std::string line : data.second)
            f << line << '\n';
        f.close();
    }
}


int main(int argc, char* argv[])
{
    std::cout << "UCOM decoder" << std::endl;
    Args args{argc, argv};
    std::map<int, json> messages;
    std::map<int, UcomMessage> ucom;
    std::map<int, std::vector<std::string>> all_data;
    
    // Create dbs object from file
    UcomDbs dbs("mobile.dbs");
    std::string dbu;
    if (args.size() > 0) {
        for (auto arg : args)
            std::cout << arg.first << " : " << arg.second << std::endl;
        
        std::string s;
        args.get_arg(std::string("-z"), s);
        std::cout << "Value: " << s << std::endl;

        
        if (args.get_arg(std::string("-u"), dbu))
        {
            std::cout << "Using .dbu file: " << dbu << std::endl;

            std::ifstream f(dbu);
            json data = json::parse(f);
            std::cout << "Available messages: " << std::endl;

            for (auto message : data["Messages"]) {
                
                //std::string header = std::move(create_header(message));
                //std::cout << "[" << message["MessageID"] << "]: " << header << std::endl;
                UcomMessage ucom_msg{message};
                messages.insert(std::make_pair(message["MessageID"].get<int>(), message));
                ucom.insert(std::make_pair(message["MessageID"].get<int>(), ucom_msg));
                if (ucom_msg.is_valid())
                    std::cout << "[ID: " << ucom_msg.get_id() << "] [Count: " << ucom_msg.get_signal_count() << "]: " << ucom_msg.get_header() << std::endl;
            }
            
        }
    }

    int sockfd = create_socket();

    if (sockfd == -1)
        return -1;

    uint8_t buffer[4096];

    int len = 0;
    while(len > -1)
    {
        len = get_data(sockfd, buffer, 4096);
        //uint16_t* id_ptr = (uint16_t*)&buffer[2];
        UcomDbu the_dbu(dbu);
        for (auto signal : the_dbu.get_signals(0))
                std::cout << signal->get_signal_id() << std::endl;

        UCOMData data{buffer, len, the_dbu};
        if (data.IsValid()) {
            std::cout << "Message: " << " ID: " << data.get_message_id() 
            << " Version: " << data.get_message_version()
            << " Time Frame: " << (uint16_t)data.get_time_frame() 
            << " Arb. Time: " << data.get_arbitrary_time() 
            << " Payload length: " << data.get_payload_length()  
            << " Signal count: " << data.get_signal_count() 
            << " CRC (calc.): " << data.get_calc_crc() << std::endl;
            if (messages.find(data.get_message_id()) != messages.end())
            {
                if (all_data.find(data.get_message_id()) == all_data.end())
                    all_data.insert(std::make_pair(data.get_message_id(), std::vector<std::string>()));
                
                // Store the UCOM data (csv format) in the relevant location
                all_data[data.get_message_id()].push_back(data.get_csv());
                
                // for (auto line : all_data[data.get_message_id()])
                //    std::cout << line << std::endl;
            }
            std::cout << data.get_csv() << std::endl;

            
        }
        else
        {
            std::cout << "Message: Invalid" << std::endl;
            len = -1;
        }
    }

    write_csv(all_data, ucom);

}



