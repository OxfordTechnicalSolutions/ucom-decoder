#pragma once 

#include "nlohmann/json.hpp"
#include <iostream>
#include "ucom_signal.hpp"

using json = nlohmann::json;

class UcomMessage : public json {
    private:
    int _signal_count = 0;
    std::string _header;
    std::string create_header();
    std::vector<ucom_signal_ptr_t> _signals;
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
    public:
    UcomMessage() {};
    UcomMessage(json message);

    bool is_valid() { return _message_id >= 0; }

    //! \brief Gets the message ID
    int get_id() { return _message_id; }

    std::string get_header() const { return _header; }
    int get_signal_count() const { return _signals.size(); } 
    const std::vector<ucom_signal_ptr_t> &get_signals();
};