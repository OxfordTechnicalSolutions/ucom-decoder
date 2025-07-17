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

std::string UcomMessage::create_header()
{
    std::stringstream ss;
    ss << "Time (" << this->operator[]("MessageTiming").get<std::string>() << ")";
    _signal_count = 1;
    for (auto msg : this->operator[]("SignalsInMessage")) {
        ss << "," << msg["SignalID"].get<std::string>();
        _signal_count++;
        UcomSignal signal(msg);
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