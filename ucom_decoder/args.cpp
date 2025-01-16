#include "args.hpp"
#include "string_helpers.hpp"

#include <iostream>

/// @brief Constructor
/// Parse the command-line arguments and populate a map with
/// key-value pairs 
/// @param argc 
/// @param argv 
Args::Args(int argc, char* argv[]) {
    if (argc < 3)
        return;

#ifdef __linux__
    // Check that there are pairs of args
    if ((argc - 1) % 2 == 0)
    {
        for (int i = 1; i < argc; i += 2) {
            std::string key{argv[i]};
            std::string value{argv[i + 1]};
            trim(key);
            trim(value);
            insert(std::make_pair(key, value));
        }
    }
#elif _WIN32
    int args = 0;
    std::string key;
    std::string value;
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg.rfind('-', 0) == 0)
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
            std::cout << arg << ' ' << std::endl;
        }
        else
        {
            value += arg;
            value += ' ';
        }
    }
    if ((args > size()) && !value.empty())
    {
        trim(key);
        trim(value);
        insert(std::make_pair(key, value));
    }


#endif
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

