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

#include "ucom/ucom_dbu.hpp"
#include <fstream>

const std::string UcomDbu::JSON_KEY_DBUSCHEMAVERSION = "DBUSchemaVersion";
const std::string UcomDbu::JSON_KEY_UCOMVERSION = "UCOMVersion";
const std::string UcomDbu::JSON_KEY_DBUVERSION = "DBUVersion";
const std::string UcomDbu::JSON_KEY_POSSIBLETRIGGERTYPES = "PossibleTriggerTypes";

UcomDbu::UcomDbu() :
    _valid(false)
{

}

// @brief   Facilitates access to the contents of a .dbu (JSON) file
// @details Parses a .dbu file and provides access to the messages and signals contained within it
// for use when decoding UCOM data
// @param[in] filename  The path to the .dbu file
UcomDbu::UcomDbu(std::string filename) :
    _filename(filename)
{
    _valid = false;
    std::ifstream f(filename);
    if (f.is_open())
    {
        try {
            json data = parse(f);
            _schema = get_value(data, "$schema");

            // Original versioning
            if (data.contains(JSON_KEY_DBUSCHEMAVERSION))
                _schema_version = data[JSON_KEY_DBUSCHEMAVERSION].get<int>();

            if (data.contains(JSON_KEY_DBUVERSION))
                _dbu_version = data[JSON_KEY_DBUVERSION].get<int>();
            
            // New, unified versioning - will override original in case both are specified
            if (data.contains(JSON_KEY_UCOMVERSION))
            {
                _schema_version = data[JSON_KEY_UCOMVERSION].get<int>();
                _dbu_version = _schema_version;
            }
            _dbu_id = get_value(data, "DBUID");
            
            _dbu_name = get_value(data, "DBUName"); 
            _dbu_description = get_value(data, "DBUDescription");

            if (data.contains(JSON_KEY_POSSIBLETRIGGERTYPES))
            {
                for (uint8_t index = 0; index <= data[JSON_KEY_POSSIBLETRIGGERTYPES].size(); index++)
                {
                    std::string key = std::to_string(index);
                    if (data[JSON_KEY_POSSIBLETRIGGERTYPES].contains(key))
                        _triggers.insert(std::make_pair(static_cast<Triggers::Types>(index), data[JSON_KEY_POSSIBLETRIGGERTYPES][key]));
                }
            }
            
            for (auto message : data["Messages"])
            {
                uint16_t message_id = message["MessageID"];
                _messages.insert({ message_id, UcomMessage(message) });
                _message_ids.push_back(message_id);
            }
            _valid = true;
        }
        catch (exception &e) {
            // Any failure will render invalid
            std::cout << e.what() << std::endl;
        }
    }
}

// @brief Checks if message_id exists in the available messages
bool UcomDbu::message_id_exists(uint16_t message_id)
{
    return _messages.contains(message_id);
}

// @brief Gets a map of the messages
std::map<uint16_t, UcomMessage>& UcomDbu::get_messages()
{
    return _messages;
}

// @brief Gets a list of the message IDs
std::list<uint16_t>& UcomDbu::get_message_ids()
{
    return _message_ids;
}

// @brief Gets a message by ID 
const UcomMessage& UcomDbu::get_message(int id)
{
    if (message_id_exists(id))
        return _messages[id];
    else
        return _empty_message;
}

// @brief Gets a vector of the signals contained in a specific message (by message ID)
const std::vector<ucom_signal_ptr_t> &UcomDbu::get_signals(uint16_t message_id)
{
    if (_messages.find(message_id) != _messages.end())
        return _messages[message_id].get_signals();
    else
        return _empty_vector;
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
    if (data_type.compare("F64") == 0)
        return OxTS::Enum::BASIC_TYPE_double;
    else if (data_type.compare("EnS64") == 0)
        return OxTS::Enum::BASIC_TYPE_enum_int64_t;  
    
    return OxTS::Enum::BASIC_TYPE_UNKNOWN;
}

// @brief Gets the value of the JSON element identified by \p key
std::string UcomDbu::get_value(json json_data, std::string key)
{
    return json_data[key].get<std::string>();
}

std::string UcomDbu::get_trigger_name(Triggers::Types type) const
{
    if (_triggers.contains(type))
        return _triggers.at(type);
    else
        return "Unknown";
}