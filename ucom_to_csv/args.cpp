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

