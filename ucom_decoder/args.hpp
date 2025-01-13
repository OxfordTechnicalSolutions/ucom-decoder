#include <map> // for map
#include <string> // for string
#include "string_helpers.hpp"

class Args : public std::map<std::string, std::string> {
    public:
    Args(int argc, char* argv[]);
    Args() = delete;
    bool get_arg(const std::string &key, std::string &s);
};