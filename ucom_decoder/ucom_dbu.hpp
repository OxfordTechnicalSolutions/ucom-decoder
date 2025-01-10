#include "nlohmann/json.hpp"
#include <iostream>
#include <string>
#include "enum/BasicType.hpp"
#include "ucom_message.hpp"

using json = nlohmann::json;

class UcomDbu : public json {
private:
    bool _valid;
    std::vector<UcomMessage> _messages;
    std::string _schema; 
    int _schema_version;
    std::string _dbu_id;
    int _dbu_version;
    std::string _dbu_name;
    std::string _dbu_description;
    std::string get_value(json json_data, std::string key);
public:
    UcomDbu(std::string filename);
    const std::vector<ucom_signal_ptr_t> &get_signals(uint16_t message_id);
    static OxTS::Enum::BASIC_TYPE get_data_type(const std::string& data_type);
};