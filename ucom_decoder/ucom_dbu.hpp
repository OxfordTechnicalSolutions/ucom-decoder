#pragma once

#include "nlohmann/json.hpp"
#include <iostream>
#include <string>
#include <list>
#include "enum/BasicType.hpp"
#include "ucom_message.hpp"

using json = nlohmann::json;

class UcomDbu : public json {
private:
    bool _valid;
    std::map<uint16_t, UcomMessage> _messages;
    std::list<uint16_t> _message_ids;
    UcomMessage _empty_message;
    std::vector<ucom_signal_ptr_t> _empty_vector;
    std::string _schema; 
    int _schema_version;
    std::string _dbu_id;
    int _dbu_version;
    std::string _dbu_name;
    std::string _dbu_description;
    std::string get_value(json json_data, std::string key);
public:
    UcomDbu();
    UcomDbu(std::string filename);
    bool get_valid() { return _valid; }
    bool message_id_exists(uint16_t message_id);
    std::map<uint16_t, UcomMessage>& get_messages();
    std::list<uint16_t>& get_message_ids();
    const UcomMessage& get_message(int id);
    const std::vector<ucom_signal_ptr_t> &get_signals(uint16_t message_id);
    static OxTS::Enum::BASIC_TYPE get_data_type(const std::string& data_type);
};