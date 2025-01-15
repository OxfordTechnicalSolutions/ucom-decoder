#include "ucom_decoder_app.hpp"
#include "input_file.hpp"
#include "ucom_data.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>

UcomDecoderApp::UcomDecoderApp(int argc, char* argv[]) :
	Application(argc, argv)
{

}

int UcomDecoderApp::process_file()
{
    InputFile input(_input_filename);
    bool data_available;
    int64_t left = input.get_file_size();

    int pkt_count = 0;
    int total_bytes = 0;
    int skipped_pkts = 0;
    std::stringstream output_filename;
    output_filename << "output_" << std::setfill('0') << std::setw(3) << "file" << ".csv";
    std::cout << "Creating output file: " << output_filename.str() << std::endl;
    std::fstream f(output_filename.str(), std::ios_base::trunc | std::ios_base::out);
    if (!f.is_open())
        std::cout << "Failed to open file" << std::endl;

    int consumed = 0;
    int available = 0;
    int offset = 0;
    int it_offset = 0;
    bool need_more_data = false;
    std::vector<char> data;
    while (left > 0)
    {
        offset = consumed - available;
        std::cout << "Getting data, offset = " << offset << std::endl;
        data = input.get_data(data_available, left, offset);
        consumed = 0;
        available = data.size();
        for (auto it = data.begin(); it <= data.end();)
        {
            need_more_data = false;
            int length = UcomData::peek(reinterpret_cast<uint8_t*>(&(*it)), data.end() - it, need_more_data);
            if (length >= 20)
            {
                // Found a candidate for a valid packet - try to extract the data
                UcomData d(reinterpret_cast<uint8_t*>(&(*it)), length, _dbu);
                if (d.IsValid())
                {
                    it += length;
                    consumed += length;
                    pkt_count++;
                    f << d.get_csv() << '\n';
                    //std::cout << d.to_string() << ", Data left: " << data.end() - it <<  std::endl;
                }
                else
                {
                    it++;
                    if (it <= data.end())
                        consumed++;
                }
            }
            else
            {
                if (need_more_data)
                {
                    it_offset = data.end() - it;
                    break;
                }
                it++;
                if (it <= data.end())
                    consumed++;
            }
            it_offset = data.end() - it;
        }
        total_bytes += consumed;

    }

    std::cout << "Read " << pkt_count << " packets" << std::endl;
    std::cout << "Total bytes read " << total_bytes << std::endl;

    f.close();

    return 0;
}

int UcomDecoderApp::run() {
	std::cout << "UCOM decoder" << std::endl;

    if (_args.size() > 0) {
        if (_args.get_arg("-u", _dbu_filename))
        {
            _dbu = UcomDbu(_dbu_filename);

            if (!_dbu.get_valid())
            {
                std::cerr << "Failed to read .dbu data from: " << _dbu_filename << std::endl;
                return -1;
            }
            else
            {
                std::cout << "Using .dbu file: " << _dbu_filename << std::endl;
            }
        }
        else
        {
            std::cout << "Please specify .dbu file to use [ -u <filename> ]" << std::endl;
            return -1;
        }

        if (_args.get_arg("-f", _input_filename))
        {
            std::fstream f(_input_filename);
            if (!f.is_open())
            {
                std::cerr << "Unable to open file: " << _input_filename << std::endl;
                return -1;
            }
            else
            {
                _process_file = true;
                std::cout << "Processing file: " << _input_filename << std::endl;
                f.close();
            }
        }

        if (_process_file)
        {
            return process_file();
        }


        return 0;
    }
}
