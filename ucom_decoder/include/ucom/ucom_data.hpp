#pragma once
#include "ucom/ucom_dbu.hpp"
#include "ucom/enum/BasicType.hpp"
#include "ucom/crc.hpp"
#define raw_data_ptr_t const uint8_t *

class UcomData {
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

    template<typename T>
    static T get_data(const uint8_t* data, int offset);

    template<typename T>
    static T get_data_update_offset(const uint8_t* data, int& offset);
public:
    UcomData(const uint8_t* data, int size, UcomDbu& dbu);
    double get_data_update_offset(raw_data_ptr_t data, OxTS::Enum::BASIC_TYPE type, int& offset);
    static const int peek(const uint8_t* data, int max_size, bool& need_more_data);

    const std::string get_csv() const;

    std::string to_string();

    uint16_t get_message_id() { return _message_id; }

    uint16_t get_message_version() { return _message_version; }

    uint8_t get_time_frame() { return _time_frame; }

    int64_t get_arbitrary_time() { return _arbitrary_time; }

    uint16_t get_payload_length() { return _payload_length; }

    size_t get_signal_count() { return _values.size(); }

    uint32_t get_calc_crc() { return _calc_crc; }

    bool get_valid() { return _valid; }

    bool get(std::string signal_id, UcomDbu& dbu, double &value);
};

