#pragma once

#include <fstream>
#include "application.hpp"
#include "ucom_dbu.hpp"
#include "socket_helper.hpp"
#include <list>
#include <set>

#define DEFAULT_PORT 50487

class UcomDecoderApp : public Application
{
private:
	int process_file();
    int process_udp();
    /* Command-line arguments
    */

    // .dbu filename
    std::string _dbu_filename;

    // The number of packets to capture (zero means capture indefinitely)
    int _max_packets = -1;

    //! @brief Duration of capture in seconds
    int _duration = -1;

    // Get data only from this IP address
    std::string _filter_ip;

    // Get data only from these message IDs
    std::list<uint16_t> _message_ids;

    // Process a file rather than UDP stream (default)
    // -f <filename>
    std::string _input_filename;

    // Output file prefix
    std::string _output_file_prefix = "output_";

    // Disable user-abort
    bool _disable_user_abort = false;

    // Filtered IP collection
    std::set<std::string> _filtered_ips;

    // Skipped message ID collection
    std::set<int> _skipped_ids;

    bool _process_file = false;

    UcomDbu _dbu;

    std::map<int, std::fstream> _output_files;

    //! @brief Statistics
    int _skipped_packets = 0;
    int _invalid_packets = 0;
    int _packet_count = 0;
    int _total_bytes = 0;
    int _filtered_packets = 0;

    int get_data(Socket& socket, uint8_t* buffer, int max_len, std::string& source_ip);
    bool create_output_files();
    void close_output_files();
    bool create_output_file(const std::string& filename, int message_id, const std::string& header, std::fstream& output_stream);
    void write_csv(std::fstream& output_stream, const std::string& csv);
    int process_args();
    void print_help_text();
    void print_banner_text();
    void display_statistics();
public:
	UcomDecoderApp(int argc, char* argv[]);
	int run() override;

};