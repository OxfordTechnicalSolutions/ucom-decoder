#include <pybind11/pybind11.h>

#include "ucom/ucom_data.hpp"
#include "ucom/ucom_dbu.hpp"
#include "ucom/ucom_message.hpp"
#include "ucom/ucom_signal.hpp"

#include <tuple>

#include <pybind11/stl.h> // for map, list, vector

namespace py = pybind11;
using namespace pybind11::literals;

// UcomData
PYBIND11_MODULE(ucom_py_sdk, m) {
    m.doc() = "Ucom SDK"; // optional module docstring

    py::class_<UcomData>(m, "UcomData")
        .def(py::init<char*, int, UcomDbu&>(), "data"_a, "size"_a, "dbu"_a)
        .def_static("peek", [](char* data, int max_size, bool &need_more_data){
            int length = UcomData::peek(reinterpret_cast<const uint8_t*>(data), max_size, need_more_data);
            return std::make_tuple(length, need_more_data);
        }, "data"_a, "max_size"_a, "need_more_data"_a)
        .def("get_csv", &UcomData::get_csv)
        .def("to_string", &UcomData::to_string)
        .def("get_message_id", &UcomData::get_message_id)
        .def("get_message_version", &UcomData::get_message_version)
        .def("get_time_frame", &UcomData::get_time_frame)
        .def("get_arbitrary_time", &UcomData::get_arbitrary_time)
        .def("get_payload_length", &UcomData::get_payload_length)
        .def("get_signal_count", &UcomData::get_signal_count)
        .def("get_calc_crc", &UcomData::get_calc_crc)
        .def("get_valid", &UcomData::get_valid);

    // UcomDbu
    py::class_<UcomDbu>(m, "UcomDbu")
        .def(py::init<>())
        .def(py::init<std::string>(), "filename"_a)
        .def("get_valid", &UcomDbu::get_valid)
        .def("get_filename", &UcomDbu::get_filename)
        .def("message_id_exists", &UcomDbu::message_id_exists, "message_id"_a)
        .def("get_messages", &UcomDbu::get_messages)
        .def("get_message_ids", &UcomDbu::get_message_ids)
        .def("get_message", &UcomDbu::get_message, "message_id"_a)
        .def("get_signals", [](UcomDbu &a, int message_id) {
        std::vector<UcomSignal> signals;
        for (auto signal : a.get_signals(message_id))
            signals.push_back(UcomSignal(*signal));
        return signals;
            }, "message_id"_a)
        .def_static("get_data_type", &UcomDbu::get_data_type, "data_type"_a);

    // UcomMessage
    py::class_<UcomMessage>(m, "UcomMessage")
        .def(py::init<>())
        .def(py::init<json>(), "message"_a)
        .def("is_valid", &UcomMessage::is_valid)
        .def("get_id", &UcomMessage::get_id)
        .def("get_header", &UcomMessage::get_header)
        .def("get_signal_count", &UcomMessage::get_signal_count)
        .def("get_signals", [](UcomMessage& a) {
        std::vector<UcomSignal> signals;
        for (auto signal : a.get_signals())
            signals.push_back(UcomSignal(*signal));
        return signals;
            });

    // UcomSignal
    py::class_<UcomSignal>(m, "UcomSignal")
        .def(py::init<>())
        .def(py::init<json>(), "signal"_a)
        .def(py::init<std::string, UcomSignal::SignalType>(), "signal_id"_a, "signal_type"_a)
        .def("get_signal_id", &UcomSignal::get_signal_id)
        .def("get_data_type", &UcomSignal::get_data_type);


#define ENUM_BASIC_TYPE(X) .value(#X, OxTS::Enum:: ## X)
    // OxTS::Enum::BASIC_TYPE
    py::enum_<OxTS::Enum::BASIC_TYPE>(m, "OxTS::Enum::BASIC_TYPE")
        ENUM_BASIC_TYPE(BASIC_TYPE_void)
        ENUM_BASIC_TYPE(BASIC_TYPE_bool)
        ENUM_BASIC_TYPE(BASIC_TYPE_char)
        ENUM_BASIC_TYPE(BASIC_TYPE_float)
        ENUM_BASIC_TYPE(BASIC_TYPE_double)
        ENUM_BASIC_TYPE(BASIC_TYPE_uint8_t)
        ENUM_BASIC_TYPE(BASIC_TYPE_uint16_t)
        ENUM_BASIC_TYPE(BASIC_TYPE_uint32_t)
        ENUM_BASIC_TYPE(BASIC_TYPE_uint64_t)
        ENUM_BASIC_TYPE(BASIC_TYPE_int8_t)
        ENUM_BASIC_TYPE(BASIC_TYPE_int16_t)
        ENUM_BASIC_TYPE(BASIC_TYPE_int32_t)
        ENUM_BASIC_TYPE(BASIC_TYPE_int64_t)
        ENUM_BASIC_TYPE(BASIC_TYPE_UNKNOWN);
}