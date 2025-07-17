#include "ucom/ucom_signal.hpp"
#include "ucom/ucom_dbu.hpp"

UcomSignal::UcomSignal()
{
}

UcomSignal::UcomSignal(json signal)
{
    _signal_id = signal["SignalID"].get<std::string>();
    _source_id = signal["SourceID"].get<std::string>();
    _data_type = UcomDbu::get_data_type(signal["DataType"].get<std::string>());
    _unit = signal["Unit"].get<std::string>();
    _scale_factor = signal["ScaleFactor"].get<double>();
    _offset = signal["Offset"].get<double>();
}

UcomSignal::UcomSignal(std::string signal_id, UcomSignal::SignalType signal_type)
{
    _signal_id = signal_id;
    _signal_type = signal_type;
}
