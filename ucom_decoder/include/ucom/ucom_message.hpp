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

#include "nlohmann/json.hpp"
#include <iostream>
#include <map>
#include "ucom_signal.hpp"

using json = nlohmann::json;

class UcomMessage : public json {
    private:
    int _signal_count = 0;
    std::string _header;
    std::string create_header();
    std::vector<ucom_signal_ptr_t> _signals;
    std::map<std::string, ucom_signal_ptr_t> _signal_map;
    std::map<std::string, int> _signal_indices;
    //! \brief Message schema version.
    uint16_t _schema_version = -1;
    //! \brief Message ID.
    uint16_t _message_id = -1;
    //! \brief Message version.
    uint16_t _message_version = -1;
    //! \brief Message name.
    std::string	_name;
    //! \brief Description
    std::string	_description;
    //! \brief Timing 
    std::string	_timing;
    //! @brief  Enabled
    bool _enabled;

    static const std::string JSON_KEY_MESSAGEENABLED;

    public:
    UcomMessage() {};
    UcomMessage(json message);

    bool is_valid() { return _message_id >= 0; }

    //! \brief Gets the message ID
    int get_id() const { return _message_id; }

    //! @brief Gets the enabled status
    //! @return true, if the message is enabled; false otherwise
    bool get_enabled() const { return _enabled; }

    std::string get_header() const { return _header; }
    size_t get_signal_count() const { return _signals.size(); } 
    const std::vector<ucom_signal_ptr_t> &get_signals();
    const ucom_signal_ptr_t get_signal(std::string id);
    int get_signal_index(std::string id);
};