#include "args.hpp"
#include "string_helpers.hpp"

/// @brief Constructor
/// Parse the command-line arguments and populate a map with
/// key-value pairs 
/// @param argc 
/// @param argv 
Args::Args(int argc, char* argv[]) {
    if (argc < 3)
        return;

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

