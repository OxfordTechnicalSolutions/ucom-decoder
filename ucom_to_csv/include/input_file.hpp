#pragma once
#include <string>
#include <fstream>
#include <vector>

class InputFile {
private:
	std::string _filename;
	std::vector<char> _buffer;
	std::ifstream _filestream;
	std::streampos _file_pointer;
	uint64_t _file_size;
	bool _valid = false;
	const uint64_t BUFFER_SIZE = 8 * 1048576;
public:
	InputFile(std::string filename);
	~InputFile();
	uint64_t get_file_size();
	std::vector<char>& get_data(bool& data_available, int64_t& left, int offset = 0);
	bool is_valid() const { return _valid; }
};