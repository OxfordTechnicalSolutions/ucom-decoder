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
#include "ucom/ucom_signal.hpp"
#include "ucom/ucom_dbu.hpp"

UcomSignal::UcomSignal()
{
}

// @brief Extracts UCOM signal information from its JSON representation
UcomSignal::UcomSignal(json signal)
{
    _signal_id = signal["SignalID"].get<std::string>();
    _source_id = signal["SourceID"].get<std::string>();
    _data_type = UcomDbu::get_ucom_data_type(signal["DataType"].get<std::string>());
    _unit = signal["Unit"].get<std::string>();
    _scale_factor = signal["ScaleFactor"].get<double>();
    _offset = signal["Offset"].get<double>();
}

// @brief Constructs a UCOM signal, initialising its ID and type
UcomSignal::UcomSignal(std::string signal_id, UcomSignal::SignalType signal_type)
{
    _signal_id = signal_id;
    _signal_type = signal_type;
}
