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
        int read = _filestream.gcount();

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