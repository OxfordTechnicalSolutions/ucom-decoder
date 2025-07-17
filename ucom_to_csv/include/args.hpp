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

#include <map> // for map
#include <string> // for string
#include "ucom/string_helpers.hpp"

class Args : public std::map<std::string, std::string> {
    public:
    Args(int argc, char* argv[]);
    Args() = delete;
    bool has_arg(const std::string& key);
    bool get_arg(const std::string &key, std::string &s);
};