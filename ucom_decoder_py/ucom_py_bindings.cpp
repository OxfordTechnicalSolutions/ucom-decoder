#include <pybind11/pybind11.h>

#include "ucom/ucom_data.hpp"
#include "ucom/ucom_dbu.hpp"
#include "ucom/ucom_message.hpp"

#include <pybind11/stl.h> // for map, list, vector

namespace py = pybind11;
using namespace pybind11::literals;

// UcomData
PYBIND11_MODULE(ucom_py_sdk, m) {
    m.doc() = "Ucom SDK"; // optional module docstring

    py::class_<UcomData>(m, "UcomData")
        .def(py::init<char*, int, UcomDbu&>(), "data"_a, "size"_a, "dbu"_a)
        .def_static("peek", &UcomData::peek, "data"_a, "max_size"_a, "need_more_data"_a)
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
        .def("get_signals", &UcomDbu::get_signals, "message_id"_a)
        .def_static("get_data_type", &UcomDbu::get_data_type, "data_type"_a);

// UcomMessage
    py::class_<UcomMessage>(m, "UcomMessage")
        .def(py::init<>())
        .def(py::init<json>(), "message"_a)
        .def("is_valid", &UcomMessage::is_valid)
        .def("get_id", &UcomMessage::get_id)
        .def("get_header", &UcomMessage::get_header)
        .def("get_signal_count", &UcomMessage::get_signal_count)
        .def("get_signals", &UcomMessage::get_signals);
/*
    m.def("add2", &add, "i"_a=1, "j"_a);

    
    */
}