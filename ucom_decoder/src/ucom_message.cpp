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
    for (json signal : message["SignalsInMessage"])
    {                  
        _signals.push_back(std::make_shared<UcomSignal>(signal));
    }

    _header = create_header();
}

const std::vector<ucom_signal_ptr_t> &UcomMessage::get_signals()
{
    return _signals;
}

std::vector<UcomSignal> &UcomMessage::get_signals_copy()
{
    if (_signals_copy.size() == 0)
    {
        for (auto signal : _signals)
            _signals_copy.push_back(UcomSignal(*signal));
    }
    return _signals_copy;
}