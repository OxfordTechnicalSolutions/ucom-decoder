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
#include <string>
#include <list>
#include "enum/BasicType.hpp"
#include "ucom_message.hpp"
#include "ucom/ucom_triggers.hpp"

#include "ucom/ucom_value.hpp"

using json = nlohmann::json;

class UcomDbu : public json {
private:
    bool _valid;
    std::map<uint32_t, UcomMessage> _messages;
    std::list<uint16_t> _message_ids;
    std::list<uint32_t> _message_uids;
    UcomMessage _empty_message;
    std::vector<ucom_signal_ptr_t> _empty_vector;
    std::string _schema; 
    int _schema_version;
    std::string _dbu_id;
    int _dbu_version;
    std::string _dbu_name;
    std::string _dbu_description;
    std::string get_value(json json_data, std::string key);
    std::string _filename;
    std::map<UCOM::TRIGGER_TYPES, std::string> _triggers;
    std::map<std::string, std::string> _header_timings;
    static const std::string JSON_KEY_DBUSCHEMAVERSION;
    static const std::string JSON_KEY_UCOMVERSION;
    static const std::string JSON_KEY_DBUVERSION;
    static const std::string JSON_KEY_POSSIBLETRIGGERTYPES;
    static const std::string JSON_KEY_POSSIBLEHEADERTIMING;
public:
    UcomDbu();
    UcomDbu(std::string filename);
    std::string get_filename() { return _filename; }
    bool get_valid() { return _valid; }
    bool message_id_exists(uint16_t message_id);
    bool message_uid_exists(uint32_t message_uid);
    std::map<uint32_t, UcomMessage>& get_messages();
    std::list<uint16_t>& get_message_ids();
    std::list<uint32_t>& get_message_uids();    // A list of unique message identifiers (a combination of ID and version)
    const UcomMessage& get_message(uint32_t uid);
    const std::vector<ucom_signal_ptr_t> &get_signals(uint32_t message_uid);
    static OxTS::Enum::BASIC_TYPE get_data_type(const std::string& data_type);
    static UCOM::DATA_TYPE get_ucom_data_type(const std::string& data_type);
    std::string get_trigger_name(UCOM::TRIGGER_TYPES type) const;
    std::string get_header_timing_name(const std::string& timing) const;

};