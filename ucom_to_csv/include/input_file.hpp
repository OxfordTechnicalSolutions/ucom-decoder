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