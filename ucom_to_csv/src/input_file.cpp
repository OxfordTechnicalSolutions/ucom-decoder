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
#include "input_file.hpp"
#include <iostream>

InputFile::InputFile(std::string filename) :
	_filename(filename)
{
    _filestream = std::ifstream(filename, std::ios::binary);
    _valid = _filestream.good();
    if (_valid)
    {
        _file_size = get_file_size();
        _file_pointer = _filestream.tellg();
    }
    _buffer.reserve(BUFFER_SIZE);
}

InputFile::~InputFile()
{
    if (_filestream.is_open())
        _filestream.close();

}

/// <summary>
/// Get the size of the file encapsulated file
/// </summary>
/// <returns></returns>
uint64_t InputFile::get_file_size()
{
    if (_filestream)
    {
        _filestream.seekg(0, _filestream.end);
        std::streampos size = _filestream.tellg();
        _filestream.seekg(0, _filestream.beg);
        return size;
    }
    else
        return 0;
}

std::vector<char>& InputFile::get_data(bool &data_available, int64_t& left, int offset)
{    
    if (_filestream.good())
    {
        // Offset adjusts the position of the filepointer for the next read 
        _file_pointer = _filestream.tellg();
        if (offset != 0)
        {
            if (offset < 0)
            {
                if (abs(offset) > _file_pointer)
                    _filestream.seekg(0, _filestream.beg);
                else
                    _filestream.seekg(offset, _filestream.cur);
            }
            else
            {
                _filestream.seekg(offset, _filestream.cur);
            }


            _file_pointer = _filestream.tellg();
        }

        _buffer.resize(_buffer.capacity());
        _filestream.read(&_buffer[0], _buffer.capacity());
        std::streamsize read = _filestream.gcount();

        if (_filestream.eof())
        {
            // End-of-file
            left = 0;
        }
        else
            left = _file_size - _filestream.tellg();

        // Resize vector to match data read
        _buffer.resize(read);
        data_available = true;
    }
    else
        data_available = false;

    return _buffer;
}