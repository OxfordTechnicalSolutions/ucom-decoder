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
    OxTS::Enum::BASIC_TYPE _data_type = OxTS::Enum::BASIC_TYPE_UNKNOWN;
    SignalType _signal_type;
    public:
    UcomSignal();
    UcomSignal(json signal);
    UcomSignal(std::string signal_id, UcomSignal::SignalType signal_type);
    std::string get_signal_id() { return _signal_id; }
    const OxTS::Enum::BASIC_TYPE get_data_type() { return _data_type; } 
    void enter() {};
    void exit() {};
};

using ucom_signal_ptr_t = std::shared_ptr<UcomSignal>;
