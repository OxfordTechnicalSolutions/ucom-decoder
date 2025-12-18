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

#include <pybind11/pybind11.h>

#include "ucom/ucom_data.hpp"
#include "ucom/ucom_dbu.hpp"
#include "ucom/ucom_message.hpp"
#include "ucom/ucom_signal.hpp"
#include "ucom/ucom_triggers.hpp"

#include <tuple>

#include <pybind11/stl.h> // for map, list, vector
#include <pybind11/native_enum.h>   // for native_enum

namespace py = pybind11;
using namespace pybind11::literals;

// UcomData
PYBIND11_MODULE(ucompy, m) {
    m.doc() = "Ucom Python SDK"; // optional module docstring

    py::class_<UcomData>(m, "UcomData")
        .def(py::init([](char* a, int b, UcomDbu& c) {
            return UcomData(reinterpret_cast<const uint8_t*>(a), b, c);
            })
            , "data"_a, "size"_a, "dbu"_a)
        .def_static("peek", [](char* data, int max_size){
        bool need_more_data;
            int length = UcomData::peek(reinterpret_cast<const uint8_t*>(data), max_size, need_more_data);
            return std::make_tuple(length, need_more_data);
        }, "data"_a, "max_size"_a)
        .def("get_csv", &UcomData::get_csv, "dbu"_a)
        .def("to_string", &UcomData::to_string)
        .def("get_message_id", &UcomData::get_message_id)
        .def("get_message_uid", &UcomData::get_message_uid)
        .def("get_message_version", &UcomData::get_message_version)
        .def("get_time_frame", &UcomData::get_time_frame)
        .def("get_arbitrary_time", &UcomData::get_arbitrary_time)
        .def("get_payload_length", &UcomData::get_payload_length)
        .def("get_signal_count", &UcomData::get_signal_count)
        .def("get_calc_crc", &UcomData::get_calc_crc)
        .def("get_valid", &UcomData::get_valid)
        .def("get", [](UcomData& a, std::string signal_id, UcomDbu& dbu) {
        valueVariant value;
        bool success = a.get(signal_id, dbu, value);
        return std::make_tuple(success, value);
            });

    // UcomDbu
    py::class_<UcomDbu>(m, "UcomDbu")
        .def(py::init<>())
        .def(py::init<std::string>(), "filename"_a)
        .def("get_valid", &UcomDbu::get_valid)
        .def("get_filename", &UcomDbu::get_filename)
        .def("message_id_exists", &UcomDbu::message_id_exists, "message_id"_a)
        .def("message_uid_exists", &UcomDbu::message_uid_exists, "message_uid"_a)
        .def("get_messages", &UcomDbu::get_messages)
        .def("get_message_ids", &UcomDbu::get_message_ids)
        .def("get_message_uids", &UcomDbu::get_message_uids)
        .def("get_message", &UcomDbu::get_message, "message_id"_a)
        .def("get_signals", [](UcomDbu &a, int message_id) {
        std::vector<UcomSignal> signals;
        for (auto signal : a.get_signals(message_id))
            signals.push_back(UcomSignal(*signal));
        return signals;
            }, "message_id"_a)
        .def_static("get_data_type", &UcomDbu::get_data_type, "data_type"_a)
        .def_static("get_ucom_data_type", &UcomDbu::get_ucom_data_type, "data_type"_a)
        .def("get_trigger_name", &UcomDbu::get_trigger_name, "type"_a)
        .def("get_header_timing_name", &UcomDbu::get_header_timing_name, "timing"_a);

    // UcomMessage
    py::class_<UcomMessage>(m, "UcomMessage")
        .def(py::init<>())
        .def(py::init<json>(), "message"_a)
        .def(py::init([](std::string s) {
        nlohmann::json j = json::parse(s);
        return new UcomMessage(j);
            }))
        .def("is_valid", &UcomMessage::is_valid)
        .def("get_id", &UcomMessage::get_id)
        .def("get_uid", &UcomMessage::get_uid)
        .def("get_header", &UcomMessage::get_header)
        .def("get_signal_count", &UcomMessage::get_signal_count)
        .def("get_enabled", &UcomMessage::get_enabled)
        .def("get_signal_index", &UcomMessage::get_signal_index, "id"_a)
        .def("get_signals", [](UcomMessage& a) {
        std::vector<UcomSignal> signals;
        for (auto signal : a.get_signals())
            signals.push_back(UcomSignal(*signal));
        return signals;
            })
        .def_static("get_id_from_uid", &UcomMessage::get_id_from_uid, "uid"_a)
        .def_static("get_version_from_uid", &UcomMessage::get_version_from_uid, "uid"_a)
        .def_static("create_uid", &UcomMessage::create_uid, "message_id"_a, "message_version"_a)
        .def_static("uid_to_string", &UcomMessage::uid_to_string, "uid"_a)
        .def_static("uid_from_string", [](std::string s) {
            uint32_t uid = -1;
            bool success = UcomMessage::uid_from_string(s, uid);
            return std::make_tuple(success, uid);
            }, "s"_a);

    // UcomSignal
    py::class_<UcomSignal>(m, "UcomSignal")
        .def(py::init<>())
        .def(py::init<json>(), "signal"_a)
        .def(py::init([](std::string s) {
        nlohmann::json j = json::parse(s);
        return new UcomSignal(j);
            }))
        .def(py::init<std::string, UcomSignal::SignalType>(), "signal_id"_a, "signal_type"_a)
        .def("get_signal_id", &UcomSignal::get_signal_id)
        .def("get_data_type", &UcomSignal::get_data_type);

    // valueVariant
    py::class_<valueVariant>(m, "valueVariant")
        .def(py::init<>())
        .def(py::init<UCOM::DATA_TYPE>(), "v_data_type"_a)
        .def(py::init<const valueVariant&>())
        .def("get_value", &valueVariant::get_value, py::return_value_policy::reference)
        .def("get_type", &valueVariant::get_type)
        .def("to_string", &valueVariant::to_string)
        .def("__eq__", &valueVariant::operator==);

    // Value
#define VALUE_GETTER(X) .def_property(#X, [](valueVariant::Value& value) { return value.X; }, nullptr)
    py::class_<valueVariant::Value>(m, "valueVariant::Value")
        .def(py::init<>())
        .def_property("ens64",
            [](valueVariant::Value& value)
            {
                return std::make_tuple(value.ens64.enum_member, value.ens64.value);
            },
            nullptr)
        VALUE_GETTER(u8)
        VALUE_GETTER(s8)
        VALUE_GETTER(u16)
        VALUE_GETTER(s16)
        VALUE_GETTER(u32)
        VALUE_GETTER(s32)
        VALUE_GETTER(u64)
        VALUE_GETTER(s64)
        VALUE_GETTER(f32)
        VALUE_GETTER(f64)
        VALUE_GETTER(str);


    // UCOM::DATA_TYPE
#define UCOM_DATA_TYPE(X) .value(#X, UCOM::DATA_TYPE::X)
    py::native_enum<UCOM::DATA_TYPE>(m, "UCOM_DATA_TYPE", "enum.Enum")
        UCOM_DATA_TYPE(STR)    ///< String
        UCOM_DATA_TYPE(U8)     ///< Unsigned 8-bit integer
        UCOM_DATA_TYPE(S8)     ///< Signed 8-bit integer
        UCOM_DATA_TYPE(U16)    ///< Unsigned 16-bit integer
        UCOM_DATA_TYPE(S16)    ///< Signed 16-bit integer
        UCOM_DATA_TYPE(U32)    ///< Unsigned 32-bit integer
        UCOM_DATA_TYPE(S32)    ///< Signed 32-bit integer
        UCOM_DATA_TYPE(U64)    ///< Unsigned 64-bit integer
        UCOM_DATA_TYPE(S64)    ///< Signed 64-bit integer
        UCOM_DATA_TYPE(F32)    ///< 32-bit floating point
        UCOM_DATA_TYPE(F64)    ///< 64-bit floating point (double)
        UCOM_DATA_TYPE(EnS64)  ///< Enum unsigned 8-bit value and a signed 64-bit integer (72-bit total)
        UCOM_DATA_TYPE(INVALID) ///< Invalid or unknown type
        .export_values()
        .finalize();

    // UCOM::Triggers::Types
#define UCOM_TRIGGERS(X) .value(#X, UCOM::TRIGGER_TYPES::X)
    py::native_enum<UCOM::TRIGGER_TYPES>(m, "UCOM_TRIGGERS", "enum.Enum")
        UCOM_TRIGGERS(NO_TRIGGER)
        UCOM_TRIGGERS(IN_1_DOWN)
        UCOM_TRIGGERS(IN_1_UP)
        UCOM_TRIGGERS(OUT_1)
        UCOM_TRIGGERS(IN_2_DOWN)
        UCOM_TRIGGERS(IN_2_UP)
        UCOM_TRIGGERS(UNKNOWN_1)
        UCOM_TRIGGERS(UNKNOWN_2)
        UCOM_TRIGGERS(OUT_2)
        .export_values()
        .finalize();

}

