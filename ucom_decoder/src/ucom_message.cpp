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

#include "ucom/ucom_message.hpp"

const std::string UcomMessage::JSON_KEY_MESSAGEENABLED = "MessageEnabled";

std::string UcomMessage::create_header()
{
    std::stringstream ss;
    ss << "Time (" << this->operator[]("MessageTiming").get<std::string>() << ")";
    _signal_count = 1;
    for (auto msg : this->operator[]("SignalsInMessage")) {
        UcomSignal signal(msg);
        _signal_count++;
        if (signal.get_data_type() == UCOM::DATA_TYPE::EnS64)
            ss << "," << "Enum element";
        ss << "," << msg["SignalID"].get<std::string>();
        if (this->operator[]("OutputType").get<std::string>().compare("OnTrigger") == 0)
            ss << ",Trigger Type";
    }
    return ss.str();
}

// @brief Extracts UCOM message information from its JSON representation
UcomMessage::UcomMessage(json message) : json(message)
{
    _message_id = message["MessageID"];
    _schema_version =  message["MessageSchemaVersion"];
    _message_id = message["MessageID"];
    _message_version = message["MessageVersion"];
    _name = message["MessageName"];
    _description = message["MessageDescription"];
    _timing = message["MessageTiming"];
    _enabled = message[JSON_KEY_MESSAGEENABLED];
    _message_uid = _message_id | _message_version << 16;

    std::vector<ucom_signal_ptr_t> signals;
    // Add all signals to collection
    int index = 0;
    for (json signal : message["SignalsInMessage"])
    {                  
        ucom_signal_ptr_t ptr = std::make_shared<UcomSignal>(signal);
        _signals.push_back(ptr);
        _signal_map.insert({ ptr->get_signal_id(), ptr });
        _signal_indices.insert({ ptr->get_signal_id(), index++ });
    }

    _header = create_header();
}

// @brief Gets a vector of UCOM signals from the message
const std::vector<ucom_signal_ptr_t> &UcomMessage::get_signals()
{
    return _signals;
}

// @brief Gets a pointer to a signal, identified by signal ID
// @param id Signal ID
// @return ucom_signal_ptr_t if signal exists, nullptr otherwise
const ucom_signal_ptr_t UcomMessage::get_signal(std::string id)
{
    if (_signal_map.contains(id))
        return _signal_map[id];
    else
        return nullptr;
}

// @brief Gets the zero-based index of a signal (in the order they are listed in the .dbu file)
// @param id Signal ID
// @return Index of signal if it exists, -1 otherwise
int UcomMessage::get_signal_index(std::string id)
{
    if (_signal_indices.contains(id))
        return _signal_indices[id];
    else
        return -1;
}

// @brief Gets a string representation of a message UID
// in the form <ID>v<VERSION>, e.g. 64520v1 for ID 64520, VERSION 1, except 
// for VERSION 0, which is is just the message ID, e.g. 64520.
// @param uid Message UID
// @return String representation of UID
const std::string UcomMessage::uid_to_string(const uint32_t uid)
{
    std::stringstream ss;
    uint16_t version = get_version_from_uid(uid);
    if (version == 0)
        ss << get_id_from_uid(uid);
    else
        ss << get_id_from_uid(uid) << "v" << version;
    return ss.str();
}

// @brief Parses a message UID from a string representation of the form
// <ID>v<VERSION>, e.g. 64520v1. If just the ID is present (e.g. 64520), then the 
// version is assumed to be 0
// @param[in]   s   String representation of the message UID
// @param[out]  uid The message UID
// @ return True, if the string was parsed correctly, false otherwise
const bool UcomMessage::uid_from_string(const std::string s, uint32_t& uid)
{
    uid = 0;
    if (s.empty())
        return false;
    size_t pos;
    pos = s.find('v');
    
    uint16_t version = 0;
    uint16_t id = 0;
    
    // Error checking is not exhaustive, e.g. 64520v1.5 is parsed without error as 64520v1
    try
    {
        id = static_cast<uint16_t>(std::stoul(s.substr(0, pos)));

        if (pos != std::string::npos)
            version = static_cast<uint16_t>(std::stoul(s.substr(pos + 1)));
        else {
            // Check for (some) format errors, e.g. 50x6.4
            std::string id_str = std::to_string(id);
            if (s.size() != id_str.size())
                return false;
        }
    }
    catch (...) {
        return false;
    }

    uid = create_uid(id, version);
    return true;
}