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
#include "ucom/ucom_triggers.hpp"


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
    _message_uid = UcomMessage::create_uid(_message_id, _message_version);
    // Byte 5 is divided into upper and lower nibbles and contains the 
    // timing info and the trigger info 
    // Bits 0 - 3 are the time, bits 4 - 7 is the trigger type
    _time_frame = data[5] & 0x0F;       
    _trigger_type = static_cast<UCOM::TRIGGER_TYPES>((data[5] & 0xF0) >> 4);
    _trigger = _trigger_type != UCOM::TRIGGER_TYPES::NO_TRIGGER;
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
        const std::vector<ucom_signal_ptr_t> signals = dbu.get_signals(_message_uid);
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
    for (auto signal : dbu.get_signals(_message_uid))
    {
        UCOM::DATA_TYPE type = signal->get_data_type();
        uint8_t enum_member;
        valueVariant value;
        get_data_update_offset(data, type, i, enum_member, value);
        _values.push_back(value);
    }

    size_t signal_count = dbu.get_message(_message_uid).get_signal_count();
    size_t value_count = _values.size();
    _valid = value_count == signal_count;
}

/**
 * @brief Extracts and updates a value from raw data based on the specified data type and offset.
 *
 * This method reads a value from the provided raw data buffer, interprets it according to the given
 * `UCOM::DATA_TYPE`, and stores the result in a `valueVariant` object. It also updates the offset
 * to reflect the position after the extracted value and sets the `enum_member` if the type is
 * `EnS64` (enumerated signed 64-bit).
 *
 * If the type is unrecognized, the value is set to NaN.
 *
 * @param data Pointer to the raw data buffer.
 * @param type The data type to interpret from the buffer.
 * @param offset Reference to the current offset in the buffer; updated after reading.
 * @param enum_member Reference to the enum member identifier; set if type is `EnS64`.
 * @param value Reference to a valueVariant object where the extracted value will be stored.
 */
void UcomData::get_data_update_offset(raw_data_ptr_t data, UCOM::DATA_TYPE type, int& offset, uint8_t& enum_member, valueVariant& value)
{
    enum_member = 255;
    int64_t v;
    switch (type)
    {
    case UCOM::DATA_TYPE::S8:
        value = get_data_update_offset<int8_t>(data, offset);
        break;
    case UCOM::DATA_TYPE::U8:
        value = get_data_update_offset<uint8_t>(data, offset);
        break;
    case UCOM::DATA_TYPE::S16:
        value = get_data_update_offset<int16_t>(data, offset);
        break;
    case UCOM::DATA_TYPE::U16:
        value = get_data_update_offset<uint16_t>(data, offset);
        break;
    case UCOM::DATA_TYPE::S32:
        value = get_data_update_offset<int32_t>(data, offset);
        break;
    case UCOM::DATA_TYPE::U32:
        value = get_data_update_offset<uint32_t>(data, offset);
        break;
    case UCOM::DATA_TYPE::S64:
        value = get_data_update_offset<int64_t>(data, offset);
        break;
    case UCOM::DATA_TYPE::U64:
        value = get_data_update_offset<uint64_t>(data, offset);
        break;
    case UCOM::DATA_TYPE::F32:
        value = get_data_update_offset<float>(data, offset);
        break;
    case UCOM::DATA_TYPE::F64:
        value = get_data_update_offset<double>(data, offset);
        break;
    case UCOM::DATA_TYPE::EnS64:
        v = get_enum_data_update_offset_i(data, offset, enum_member);
        value.set_value(v, enum_member);
        break;
    case UCOM::DATA_TYPE::STR:
        value = get_str_data_update_offset(data, offset);
        break;
    default:
        value = nan("");
    }
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

/* @brief Gets a value of type T from the buffer (data), starting at offset
* @param[in] data       Pointer to the buffer
* @param[in] offset     The offset into the buffer where the value starts
* @return               The value (of type T)
*/
template <typename T>
T UcomData::get_data(const uint8_t* data, int offset) {
    T* ptr = (T*)&data[offset];
    return *ptr;
}

/* @brief Gets a value of type T from the buffer (data), starting at offset. Updates offset 
* to point to the location after the extracted value
* @param[in] data       Pointer to the buffer
* @param[in] offset     The offset into the buffer where the value starts
* @return               The value (of type T)
*/
template <typename T>
T UcomData::get_data_update_offset(const uint8_t* data, int& offset) {
    T* ptr = (T*)&data[offset];
    offset += sizeof(T);
    return *ptr;
}

/* @brief Gets an enum value of type T from the buffer (data), starting at offset. Updates offset
* to point to the location after the extracted value
* @param[in] data       Pointer to the buffer
* @param[in] offset     The offset into the buffer where the value starts
* @return               The value (of type T)
*/
template <typename T>
T UcomData::get_enum_data_update_offset(const uint8_t* data, int& offset, uint8_t& enum_member) {

    uint8_t* enum_ptr = (uint8_t*)&data[offset];
    enum_member = *enum_ptr;
    offset += sizeof(uint8_t);
    T* ptr = (T*)&data[offset];
    offset += sizeof(T);
    return *ptr;
}

int64_t UcomData::get_enum_data_update_offset_i(const uint8_t* data, int& offset, uint8_t& enum_member) {

    uint8_t* enum_ptr = (uint8_t*)&data[offset];
    enum_member = *enum_ptr;
    offset += sizeof(uint8_t);
    int64_t* ptr = (int64_t*)(& data[offset]);
    offset += sizeof(int64_t);
    int64_t v = *ptr;
    return *ptr;
}

/* @brief Gets a value of type string from the buffer (data), starting at offset. Updates offset
* to point to the location after the extracted value
* @param[in] data       Pointer to the buffer
* @param[in] offset     The offset into the buffer where the value starts
* @return               The value (of type T)
*/
std::string UcomData::get_str_data_update_offset(const uint8_t* data, int& offset) {
    std::string str(reinterpret_cast<const char*>(&data[offset]));
    offset += str.size() + 1;
    return str;
}


// @brief Gets the contents of the message (signal values) in CSV format
// @param dbu The DBU used to decode 
// @return A string containing the CSV
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
            ss << "," << value.to_string();
    }

    // Output the trigger type for a message that was output as a result of a trigger event
    if (_trigger)
        ss << "," << dbu.get_trigger_name(_trigger_type);

    return ss.str();
}

// @brief Gets a string containing meta-data (message ID, version and payload length)
std::string UcomData::to_string()
{
    std::stringstream ss;
    ss << "ID: " << get_message_id() << " Version: " << get_message_version() << " Payload length: " << get_payload_length();
    return ss.str();
}

//! @brief Gets the value of the signal as a valueVariant
//! @param signal_id 
//! @return True, if the signal exists, false otherwise
bool UcomData::get(std::string signal_id, UcomDbu& dbu, valueVariant& value)
{
    UcomMessage msg = dbu.get_message(get_message_id());
    int index = msg.get_signal_index(signal_id);

    if (index > -1)
    {
        value = _values[index];
        return true;
    }

    return false;
}



