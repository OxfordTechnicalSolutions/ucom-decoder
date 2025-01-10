#pragma once
#include <string>
#include "enum/BasicType.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class UcomSignal {
    public:
    enum SignalType {
        None,
        Timing,
        Single
    };
    private:
    std::string _source_id;
    std::string _signal_id;
    std::string _unit;
    double _scale_factor;
    double _offset;
    OxTS::Enum::BASIC_TYPE _data_type;
    SignalType _signal_type;
    public:
    UcomSignal();
    UcomSignal(json signal);
    UcomSignal(std::string signal_id, UcomSignal::SignalType signal_type);
    std::string get_signal_id() { return _signal_id; }
    const OxTS::Enum::BASIC_TYPE get_data_type() { return _data_type; } 
};

using ucom_signal_ptr_t = std::shared_ptr<UcomSignal>;
