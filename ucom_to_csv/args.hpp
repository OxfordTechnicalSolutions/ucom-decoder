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