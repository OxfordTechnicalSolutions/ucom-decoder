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
#include "args.hpp"
#include "ucom/string_helpers.hpp"

#include <iostream>

/// @brief Constructor
/// Parse the command-line arguments and populate a map with
/// key-value pairs 
/// @param argc 
/// @param argv 
Args::Args(int argc, char* argv[]) {
    if (argc < 3)
        return;

    int args = 0;
    std::string key;
    std::string value;
    for (int i = 1; i <= argc; i++) {
        std::string arg;
        if (i < argc)
            arg = std::move(std::string(argv[i]));

        if ((arg.rfind('-', 0) == 0) || (i == argc)) 
        {
            if (args > 0)
            {
                trim(key);
                trim(value);
                insert(std::make_pair(key, value));
                value.clear();
            }
            args++;
            key = arg;
        }
        else
        {
            value += arg;
            value += ' ';
        }
    }
}

bool Args::has_arg(const std::string& key)
{
    auto it = find(key);
    return(it != end());
}

bool Args::get_arg(const std::string &key, std::string &s)
{
    auto it = find(key);
    if (it != end()) {
        s = it->second;
        return true;
    }
    return false;
}

