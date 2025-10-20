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

#include "ucom/ucom_data.hpp"
#include "ucom/ucom_value.hpp"
#include "ucom/triggers.hpp"


// @brief Encapsulates and extracts the data from a UCOM packet
// @details Parses the data according to the supplied dbu. The dbu defines the structure of the UCOM message(s)
// @param[in] data  Pointer to the data
// @param[in] size  Number of bytes
// @param[in] dbu   Reference to the dbu    
UcomData::UcomData(const uint8_t* data, int size, UcomDbu& dbu) :
    _error_no{ 0 },
    _error_msgs{},
    _trigger{ false }
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
    // Byte 5 is now divided into upper and lower nibbles and contains the 
    // timing info and the trigger info 
    // Bits 0 - 3 are the time, bits 4 - 7 is the trigger type
    _time_frame = data[5] & 0x0F;       
    _trigger_type = static_cast<Triggers::Types>((data[5] & 0xF0) >> 4);
    _trigger = _trigger_type != Triggers::Types::NO_TRIGGER;
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

    int i = 16;
    // Check if this is an error message
    if (_message_id == ERROR_MSG_ID)
    {
        // Hard-coded error message structure
        const std::vector<ucom_signal_ptr_t> signals = dbu.get_signals(_message_id);
        if (signals.size() != 2)
            return;

        // Get the error number
        _error_no = get_data<uint8_t>(data, i);
        i += 1;
        // Get the error messages string
        _error_msgs = std::string(reinterpret_cast<const char*>(&data[i]), _payload_length - (i - 16));
        return;
    }

    // Step through the signals and decode according to their type
    for (auto signal : dbu.get_signals(_message_id))
    {
        OxTS::Enum::BASIC_TYPE type = signal->get_data_type();
        uint8_t enum_member;
        double value = get_data_update_offset(data, type, i, enum_member);
        
        switch (type)
        {
        case OxTS::Enum::BASIC_TYPE_enum_int64_t:
        {
            UCOM::EnS64 v(enum_member, static_cast<int64_t>(value));
            valueVariant ucom_value(UCOM::DATA_TYPE::EnS64);
            ucom_value.value.ens64 = v;
            _values.push_back(ucom_value);
        }
        break;
        default:
            valueVariant ucom_value(UCOM::DATA_TYPE::F64);
            ucom_value.value.f64 = value;
            _values.push_back(ucom_value);
            break;
        }
    }

    size_t signal_count = dbu.get_message(_message_id).get_signal_count();
    size_t value_count = _values.size();
    _valid = value_count == signal_count;
}

double UcomData::get_data_update_offset(raw_data_ptr_t data, OxTS::Enum::BASIC_TYPE type, int& offset, uint8_t& enum_member)
{
    double value;
    enum_member = 255;
    
    switch (type)
    {
    case OxTS::Enum::BASIC_TYPE_bool:
        value = get_data_update_offset<bool>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE_int8_t:
        value = get_data_update_offset<int8_t>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE_uint8_t:
        value = get_data_update_offset<uint8_t>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE_int16_t:
        value = get_data_update_offset<int16_t>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE_uint16_t:
        value = get_data_update_offset<uint16_t>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE_int32_t:
        value = get_data_update_offset<int32_t>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE_uint32_t:
        value = get_data_update_offset<uint32_t>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE::BASIC_TYPE_int64_t:
        value = get_data_update_offset<int64_t>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE::BASIC_TYPE_uint64_t:
        value = get_data_update_offset<uint64_t>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE::BASIC_TYPE_float:
        value = get_data_update_offset<float>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE::BASIC_TYPE_double:
        value = get_data_update_offset<double>(data, offset);
        break;
    case OxTS::Enum::BASIC_TYPE_enum_int64_t:
        value = get_enum_data_update_offset<int64_t>(data, offset, enum_member);
        break;
    default:
        value = nan("");
    }
    return value;
}

/* @brief Inspect data to determine if it contains a candidate UCOM packet
* @param[in] data       Pointer to the data to inspect
* @param[in] max_size   The maximum number of bytes to inspect 
* @param[out] need_more_data    Potential packet found, but more data required for a full packet
* @return
*   -1 : if no candidate found or if potential candidate found but 
*        more data is needed (\p need_more_data is true)
*   packet length : if candidate found 
*/
const int UcomData::peek(const uint8_t* data, int max_size, bool& need_more_data)
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
    {
        // Insufficient data available for a full packet
        need_more_data = true;
        return -1;
    }

    return payload_length + 20;
}


template <typename T>
T UcomData::get_data(const uint8_t* data, int offset) {
    T* ptr = (T*)&data[offset];
    return *ptr;
}

template <typename T>
T UcomData::get_data_update_offset(const uint8_t* data, int& offset) {
    T* ptr = (T*)&data[offset];
    offset += sizeof(T);
    return *ptr;
}

template <typename T>
T UcomData::get_enum_data_update_offset(const uint8_t* data, int& offset, uint8_t& enum_member) {

    uint8_t* enum_ptr = (uint8_t*)&data[offset];
    enum_member = *enum_ptr;
    offset += sizeof(uint8_t);
    T* ptr = (T*)&data[offset];
    offset += sizeof(T);
    return *ptr;
}

const std::string UcomData::get_csv(const UcomDbu& dbu) const
{
    std::stringstream ss;
    ss.precision(7);
    ss << _arbitrary_time;
    if (_message_id == ERROR_MSG_ID)
    {
        ss << "," << std::to_string(_error_no);
        ss << "," << _error_msgs;
    }
    else
    {
        for (valueVariant value : _values)
        {
            switch (value.value_type)
            {
            case UCOM::DATA_TYPE::EnS64:
                ss << "," << (int)value.value.ens64.enum_member << "," << std::fixed << value.value.ens64.value;
                //ss << "," << value.to_string();
                break;
            default:
                ss << "," << value.to_string();
            }
        }
    }

    // T denotes that message was output as a result of a trigger event
    if (_trigger)
        ss << "," << dbu.get_trigger_name(_trigger_type);

    return ss.str();
}

std::string UcomData::to_string()
{
    std::stringstream ss;
    ss << "ID: " << get_message_id() << " Version: " << get_message_version() << " Payload length: " << get_payload_length();
    return ss.str();
}

//! @brief Gets the value of the signal
//! @param signal_id 
//! @return True, if the signal exists, false otherwise
bool UcomData::get(std::string signal_id, UcomDbu &dbu, double &value)
{
    UcomMessage msg = dbu.get_message(get_message_id());
    int index = msg.get_signal_index(signal_id);

    if (index > -1)
    {
        if (_values[index].value_type == UCOM::DATA_TYPE::F64)
        value = _values[index].value.f64;
        return true;
    }

    return false;
}

