#include "ucom_dbu.hpp"
#include <fstream>

UcomDbu::UcomDbu() :
    _valid(false)
{

}

UcomDbu::UcomDbu(std::string filename)
{
    _valid = false;
    std::ifstream f(filename);
    if (f.is_open())
    {
        try {
            //this->emplace(parse(f));
            //this->operator[]("dbu") = parse(f);
            json data = parse(f);
            _schema = get_value(data, "$schema");
            _schema_version = data["DBUSchemaVersion"].get<int>();
            _dbu_id = get_value(data, "DBUID");
            _dbu_version = data["DBUVersion"].get<int>(); 
            _dbu_name = get_value(data, "DBUName"); 
            _dbu_description = get_value(data, "DBUDescription");
            
            for (auto message : data["Messages"])
            {
                uint16_t message_id = message["MessageID"];
                _messages.insert({ message_id, UcomMessage(message) });
                _message_ids.push_back(message_id);
            }
            _valid = true;
        }
        catch (...) {
            // TODO - review exception handling
        }
    }
}

// Check if message_id exists in the available messages
bool UcomDbu::message_id_exists(uint16_t message_id)
{
    return _messages.contains(message_id);
}

std::map<uint16_t, UcomMessage>& UcomDbu::get_messages()
{
    return _messages;
}

std::list<uint16_t>& UcomDbu::get_message_ids()
{
    return _message_ids;
}

const UcomMessage& UcomDbu::get_message(int id)
{
    if (message_id_exists(id))
        return _messages[id];
    else
        return _empty_message;
}

const std::vector<ucom_signal_ptr_t> &UcomDbu::get_signals(uint16_t message_id)
{
    if (_messages.find(message_id) != _messages.end())
        return _messages[message_id].get_signals();
    else
        return std::vector<ucom_signal_ptr_t>();
}

OxTS::Enum::BASIC_TYPE UcomDbu::get_data_type(const std::string& data_type)
{
    /* "SupportedTypes" from signal.schema.json:
        "B1",
        "S8",
        "U8",
        "S16",
        "U16",
        "U24",
        "S32",
        "U32",
        "S64",
        "U64",
        "F32",
        "F64"
        */


    if (data_type.compare("B1") == 0)
        return OxTS::Enum::BASIC_TYPE_bool;
    if (data_type.compare("S8") == 0)
        return OxTS::Enum::BASIC_TYPE_int8_t;
    if (data_type.compare("U8") == 0)
        return OxTS::Enum::BASIC_TYPE_uint8_t;
    if (data_type.compare("S16") == 0)
        return OxTS::Enum::BASIC_TYPE_int16_t;
    if (data_type.compare("U16") == 0)
        return OxTS::Enum::BASIC_TYPE_uint16_t;
    //if (data_type.compare("U24") == 0)    // Doesn't currently exist in enum 
    //    return OxTS::Enum::BASIC_TYPE_uint24_t;
    if (data_type.compare("S32") == 0)
        return OxTS::Enum::BASIC_TYPE_int32_t;
    if (data_type.compare("U32") == 0)
        return OxTS::Enum::BASIC_TYPE_uint32_t;
    if (data_type.compare("S64") == 0)
        return OxTS::Enum::BASIC_TYPE_int64_t;
    if (data_type.compare("U64") == 0)
        return OxTS::Enum::BASIC_TYPE_uint64_t;
    if (data_type.compare("F32") == 0)
        return OxTS::Enum::BASIC_TYPE_float;
    else if (data_type.compare("F64") == 0)
        return OxTS::Enum::BASIC_TYPE_double;
    
    return OxTS::Enum::BASIC_TYPE_UNKNOWN;
}

std::string UcomDbu::get_value(json json_data, std::string key)
{
    return json_data[key].get<std::string>();
}