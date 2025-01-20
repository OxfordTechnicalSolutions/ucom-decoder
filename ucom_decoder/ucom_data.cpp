#include "ucom_data.hpp"

UcomData::UcomData(const uint8_t* data, int size, UcomDbu& dbu)
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

    int i = 16;
    // Step through the signals and decode according to their type
    _values.clear();
    for (auto signal : dbu.get_signals(_message_id))
    {
        OxTS::Enum::BASIC_TYPE type = signal->get_data_type();
        _values.push_back(get_data_update_offset(data, type, i));
    }

    size_t signal_count = dbu.get_message(_message_id).get_signal_count();
    size_t value_count = _values.size();
    _valid = value_count == signal_count;
}

double UcomData::get_data_update_offset(raw_data_ptr_t data, OxTS::Enum::BASIC_TYPE type, int& offset)
{
    double value;
    
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
    default:
        value = nan("");
    }
    return value;
}

/* Inspect data to determine if it contains a candidate UCOM packet
* Returns:
*   -1 : if no candidate found or if potential candidate found but 
*        more data is needed (need_more_data is true)
*   packet length : if candidate found* 
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

const std::string UcomData::get_csv() const
{
    std::stringstream ss;
    ss.precision(7);
    ss << _arbitrary_time;
    for (double value : _values)
        ss << "," << std::fixed << value;
    return ss.str();
}

std::string UcomData::to_string()
{
    std::stringstream ss;
    ss << "ID: " << get_message_id() << " Version: " << get_message_version() << " Payload length: " << get_payload_length();
    return ss.str();
}