#pragma once

#include "application.hpp"
#include "ucom_dbu.hpp"
#include <list>

class UcomDecoderApp : public Application
{
private:
	int process_file();
    /* Command-line arguments
    */

    // .dbu filename
    std::string _dbu_filename;

    // The number of packets to capture (zero means capture indefinitely)
    int max_packets = -1;

    // Get data only from this IP address
    std::string _filter_ip;

    // Get data only from these message IDs
    std::list<uint16_t> _message_ids;

    // Process a file rather than UDP stream (default)
    // -f <filename>
    std::string _input_filename;
    bool _process_file = false;

    UcomDbu _dbu;
public:
	UcomDecoderApp(int argc, char* argv[]);
	int run() override;
};