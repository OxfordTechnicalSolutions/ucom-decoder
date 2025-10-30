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
#pragma once
#include "ucom/ucom_dbu.hpp"
#include "ucom/enum/BasicType.hpp"
#include "ucom/crc.hpp"
#include "ucom/ucom_value.hpp"
#include "ucom/triggers.hpp"
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
    bool _trigger; // Set if this message was output as a result of a trigger event
    Triggers::Types _trigger_type;  // Bits 3 - 7 of byte 5, the trigger type (zero if not a trigger output)
    bool _valid;
    //std::vector<double> _values; // collection for signal values
    std::vector<valueVariant> _values; // collection for signal values

    uint32_t _message_uid;  // Unique identifier

    // Errors
    uint8_t _error_no;
    std::string _error_msgs;

    template<typename T>
    static T get_data(const uint8_t* data, int offset);

    template<typename T>
    static T get_data_update_offset(const uint8_t* data, int& offset);
    template<typename T>
    static T get_enum_data_update_offset(const uint8_t* data, int& offset, uint8_t& enum_member);
    static int64_t get_enum_data_update_offset_i(const uint8_t* data, int& offset, uint8_t& enum_member);
    static std::string get_str_data_update_offset(const uint8_t* data, int& offset);
    static void get_data_update_offset(raw_data_ptr_t data, UCOM::DATA_TYPE type, int& offset, uint8_t& enum_member, valueVariant& out_value);
public:
    UcomData(const uint8_t* data, int size, UcomDbu& dbu);
    double get_data_update_offset(raw_data_ptr_t data, OxTS::Enum::BASIC_TYPE type, int& offset, uint8_t& enum_member);
    static const int peek(const uint8_t* data, int max_size, bool& need_more_data);

    const std::string get_csv(const UcomDbu& dbu) const;

    std::string to_string();

    uint16_t get_message_id() { return _message_id; }

    uint32_t get_message_uid() { return _message_uid; }

    uint16_t get_message_version() { return _message_version; }

    uint8_t get_time_frame() { return _time_frame; }

    int64_t get_arbitrary_time() { return _arbitrary_time; }

    uint16_t get_payload_length() { return _payload_length; }

    size_t get_signal_count() { return _values.size(); }

    uint32_t get_calc_crc() { return _calc_crc; }

    bool get_valid() { return _valid; }

    bool get(std::string signal_id, UcomDbu& dbu, double &value);

    bool get(std::string signal_id, UcomDbu& dbu, valueVariant& value);

    uint8_t get_error_no() { return _error_no; }

    std::string get_error_messages() { return _error_msgs; }

    static const uint16_t ERROR_MSG_ID = 0xFBFF;
};

