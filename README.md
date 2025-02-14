# UCOM Decoder

This repository contains the code to build and install the UCOM Decoder Library, 
as well as an example tool which can convert UCOM data from UDP stream or file 
to CSV file.

## Clone the repository

```sh
git clone --recursive git@github.com:OxfordTechnicalSolutions/UCOM_decoder.git
```

The repository must be cloned recursively as the UCOM decoder uses a sub-module 
for the JSON parser.

## Build

### Visual Studio
The project is set up to allow cross-platform build (Windows / Linux) using 
Visual Studio 2022. The Linux build would most commonly use WSL2, but it is 
possible to use other remote hosts or build in Linux directly.

To start, open the project folder in Visual Studio. VS should automatically 
detect the CMake files and make the configurations available in the menu.

#### Quick Start
Select the host, e.g.:
Local Machine for Windows build
WSL: Ubuntu for Linux build

Select the build configuration, e.g.:
x64 Debug

Select the 'Startup Item', e.g.:
ucom_decoder.exe

Run the selected configuration

### Linux

```sh
mkdir build && cd build
cmake ..
cmake --build .
```

To build and install the library, replace `cmake --build .` with 

```sh
cmake --build . --target install
```

If there are permission errors, re-run the above command as root with `sudo`.

The executable is found in the build/ucom_to_csv directory named `ucom_to_csv`.


## UCOM to CSV command-line options

Running UCOM to CSV with no arguments produces the following output:

```
Usage: ucom_decoder [options] -u <.dbu filename>
Options:
  -h                     Help - displays this message
  -f <input file>        Extract data from a file instead of UDP stream
  -m <id1 id2 id3 ...>   Message IDs to process, e.g. 0 1 3
  -c <packets>           Number of packets to process (UDP stream only)
  -t <duration>          Maximum capture duration in seconds (UDP stream only)
  -i <address>           Source IP address (UDP stream only)
  -o <output file>       Output file prefix (default is "output_")
  -a                     Disable user-abort
```

## Example Data

The `example_data` directory contains:

- UCOM file, logged from an OxTS INS
- .cfg file, showing how UCOM messages are configured
- oxts.dbu file (in dbu sub-directory) - contains message definitions which are parsed by the decoder. This allows the decoder flexibility to decode new messages when 
provided with updated definitions.

To use this data with the UCOM to CSV tool:

```sh
ucom_decoder <new command with dbu path option>
```
## Python
There is a Python version of the UCOM decoder SDK.

### Installation
1. Create a working folder and clone the UCOM_decoder repository:

```
mkdir ucompy_test
cd ucompy_test
git clone git@github.com:OxfordTechnicalSolutions/UCOM_decoder.git --recursive ucom_decoder
```

2. Create Python virtual environment:  
NB Use the appropriate command (instead of python) to invoke the Python interpreter on your system, e.g. py, python, python3. 
```
python -m venv venv

[Windows Command Prompt] 
venv\Scripts\activate.bat

[Windows Powershell]
venv\Scripts\Activate.ps1

[Linux]
source venv/bin/activate
```
3. Install the oxts.ucompy module:
```
python -m pip install ucom_decoder/ucom_decoder_py
```
4. Test the installation:  
a. Start Python interactive shell
```
python
```
Enter the text following the prompts (>>>) line by line. You should see the output shown:
```Python
>>> from oxts.ucompy import UcomDbu
>>> u = UcomDbu("ucom_decoder/example_data/dbu/oxts.dbu")
>>> u.get_valid()
True 
>>> u.get_filename()
'ucom_decoder/example_data/dbu/oxts.dbu'
>>> quit()
```

b. Run the ucom_to_csv example:
```
ucom_decoder\ucom_decoder_py\examples\ucom_to_csv\test.bat
```
5. Uninstall:
```
python -m pip uninstall ucompy
```

# Decoding UCOM 

## Overview
UCOM provides the facility to customise the data that is output by an OxTS INS by using user-defined messages. 
These user-defined messages are specified in a mobile.dbu file. For ease of explanation, hereafter the term DBU will be used to refer to .dbu files, such as oxts.dbu and mobile.dbu, or their content.

## UCOM
### UCOM Packet
UCOM data is sent in the form of packets that are made up of:
* 16-byte header
* Variable length payload
* CRC integrity check (32-bit)

#### Header
* Bytes 14-15 : Payload length in bytes

#### Payload
The data associated with each signal is contained in the payload as a sequence of bytes whose layout is determined by the messages defined in the DBU.

Each message contains an array of signals (named 'SignalsInMessage' in the DBU), and each signal contains a number of properties (key : value pairs). One of these properties is *DataType*. When decoding UCOM *DataType* is used to determine the size in bytes of the signal data in the payload.

Example:

[DBU excerpt]

```json
           "SignalsInMessage": [
                {
                    "SourceID": "TIME",
                    "SignalID": "GNSST",
                    "Unit": "s",
                    "ScaleFactor": 1,
                    "Offset": 0,
                    "DataType": "S64"
                },
                {
                    "SourceID": "BNS_SDN",
                    "SignalID": "Lat",
                    "Unit": "deg",
                    "ScaleFactor": 1,
                    "Offset": 0,
                    "DataType": "F64"
                },
                {
                    "SourceID": "BNS_SDN",
                    "SignalID": "Long",
                    "Unit": "deg",
                    "ScaleFactor": 1,
                    "Offset": 0,
                    "DataType": "F64"
                },

```

These three signals are each 64 bits - or 8 bytes - long (as are the vast majority of the signals), so their representation in the payload will be as follows:
```
A0 A1 A2 A3 A4 A5 A6 A7 B0 B1 B2 B3 B4 B5 B6 B7 C0 C1 C2 C3 C4 C5 C6 C7 
```
where A0 ... A7, B0 ... B7, C0 ... C7 are the bytes representing the three messages A = GNSST, B = Lat, C = Long above.

If message A is 8 bytes, message B is 1 byte and message C is 4 bytes:
```
A0 A1 A2 A3 A4 A5 A6 A7 B0 C0 C1 C2 C3
```


For full details of the UCOM protocol see **<! INSERT DOCUMENT LINK HERE !>**

### Decoding
The steps required to decode UCOM are:

1. Extract message and signal information from the DBU
2. Read UCOM data (from file, UDP etc.)  
  a. Decode the header 
  b. Decode the payload
  c. Calculate the CRC
3. Repeat from 2. as required

## ucom_to_csv
### Description
This example code demonstrates usage of the UCOM decoder SDK to decode UCOM data and write it to .csv files. 
### Windows

Clone repo (see above)

Open a 'Developer Command Prompt for VS'

Navigate to the UCOM_decoder folder and then build 

```
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

Start capturing UCOM packets from UDP:
```
cd ..
mkdir test && cd test
..\build\ucom_to_csv\Release\ucom_to_csv.exe -u ..\example_data\dbu\oxts.dbu -i any
```
#### ucom_to_csv arguments

Calling ucom_to_csv.exe with no arguments (or with -h) will display the options:
```
Usage: ucom_to_csv [options] -u <.dbu filename>
Options:
  -h                     Help - displays this message
  -f <input file>        Extract data from a file instead of UDP stream
  -m <id1 id2 id3 ...>   Message IDs to process, e.g. 0 1 3
  -c <packets>           Number of packets to process (UDP stream only)
  -t <duration>          Maximum capture duration in seconds (UDP stream only)
  -i <address>           Source IP address (UDP stream only)
  -o <output file>       Output file prefix (default is output_)
  -a                     Disable user-abort
```

The -u argument is required as it specifies the DBU that is needed for decoding the UCOM data.

Usually, when capturing packets from UDP, you will specify the source IP address using the '-i' flag, e.g. '-i 192.168.1.100' will only capture packets originating from the INS with that IP address. If there is only one INS connected to your network, then you can specify 'any' and there will be no IP filtering, i.e. use '-i any'.

## SDK
### C++ API
### class UcomDbu
#### Description
This class accepts a DBU filename as an argument to its constructor and parses the JSON contained in the DBU to generate collections of UcomMessages and UcomSignals.

<dl>
<dt>UcomDbu(std::string filename)</dt>
<dd>Constructs a UcomDbu instance from a DBU file <filename>. The file must be valid JSON conforming to the DBU schema.</dd>
<dt>std::string get_filename()</dt>
<dd>Gets the filename of the DBU.</dd>
<dt>bool get_valid()</dt>
<dd>Gets the 'valid' status of the instance. Returns true if the DBU file was parsed without error; false otherwise.</dd>
<dt>bool message_id_exists(uint16_t message_id)</dt>
<dd>Returns true if <message_id> exists in the collection of messages in the DBU; false otherwise.</dd>
<dt>std::map<uint16_t, UcomMessage>& get_messages()</dt>
<dd>Gets a collection containing all of the UCOM message definitions in the DBU, stored as key : value pairs whose key is the message ID.</dd>
<dt>std::list<uint16_t>& get_message_ids()</dt>
<dd>Gets a collection containing the message IDs of all of the UCOM message definitions in the DBU.</dd>
<dt>const UcomMessage& get_message(int id)</dt>
<dd>Gets the UcomMessage whose ID is &ltid&gt</dd>
<dt>const std::vector<ucom_signal_ptr_t> &get_signals(uint16_t message_id)</dt>
<dd>Gets a collection containing all of the signals in the UcomMessage whose ID is &ltmessage_id&gt.</dd>
<dt>static OxTS::Enum::BASIC_TYPE get_data_type(const std::string& data_type)</dt>
<dd>Gets the OxTS::Enum::BASIC_TYPE from the string representation of the type.</dd>
</dl>

### class UcomData
#### Description
This class decodes a UCOM packet and allows read-access to the signal values contained within. To decode a UCOM packet, a corresponding valid UcomDbu instance is required to provide the necessary signal layout information.

<dl>
<dt>UcomData(const uint8_t* data, int size, UcomDbu& dbu)</dt>
<dd>Constructs a UcomData instance from the byte array passed as the &ltdata&gt argument, whose length in bytes is contained in &ltsize&gt. &ltdbu&gt is a reference to a valid UcomDbu instance.</dd>
<dt>static const int peek(const uint8_t* data, int max_size, bool& need_more_data)</dt>
<dd>Inspect &ltdata&gt to determine if it contains a candidate UCOM packet.<br/>Returns:
<ul>
<li>-1 : if no candidate found or if potential candidate found but more data is needed (need_more_data is true)</li>
<li>packet length : if candidate found</li></dd>
<dt>const std::string get_csv() const</dt>
<dd>Gets the signal values as a comma-separated string</dd>
<dt>std::string to_string()</dt>
<dd>Gets a string representation of the instance, consisting of message ID, message version and payload length</dd>
<dt>uint16_t get_message_id()</dt>
<dd>Gets the message ID</dd>
<dt>uint16_t get_message_version()</dt>
<dd>Gets the message version</dd>
<dt>uint8_t get_time_frame()</dt>
<dd>Gets the time frame of the 'Arbitrary Time' value</dd>
<dt>int64_t get_arbitrary_time()</dt>
<dd>Gets the 'Arbitrary Time' value</dd>
<dt>uint16_t get_payload_length()</dt>
<dd>Gets the payload length (in bytes)</dd>
<dt>size_t get_signal_count()</dt>
<dd>Gets the signal count</dd>
<dt>uint32_t get_calc_crc()</dt>
<dd>Gets the calculated CRC</dd>
<dt>bool get_valid()</dt>
<dd>Gets the 'valid' status of the signal. Returns true if the UCOM packet has been successfully decoded; false otherwise</dd>
<dt>bool get(std::string signal_id, UcomDbu& dbu, double &value)</dt>
<dd>Gets the value of the signal whose ID is &ltsignal_id&gt as a double. The value is assigned to the argument &ltvalue&gt. Returns true if the signal value is successfully assigned; false otherwise (e.g. if the signal ID doesn't exist)</dd>
</dl>

### class UcomMessage
#### Description
Represents a UCOM message. Allows read-access to the signals contained within. Derives from *json* class.

<dl>
<dt>UcomMessage(json message)</dt>
<dd>Constructs a UcomMessage instance from a JSON representation (usually part of a DBU)</dd>
<dt>bool is_valid()</dt>
<dd>Returns true if the UcomMessage is valid; false otherwise</dd>
<dt>int get_id()</dt>
<dd>Gets the ID of the message</dd>
<dt>std::string get_header()</dt>
<dd>Gets a comma-separated string of the names of the signals contained in the message (mainly intended for use when generating CSV output). The string is prepended with 'Time (&lt<i>MessageTiming</i>&gt)', representing the '<i>Arbitrary Time</i>' time frame, e.g. <i>'Time (SDN)'</i></dd>
<dt>size_t get_signal_count()</dt>
<dd>Gets the number of signals contained in the message</dd>
<dt>const std::vector<ucom_signal_ptr_t> &get_signals()</dt>
<dd>Gets a collection of (smart, shared) pointers to the signals contained in the message</dd>
<dt>const ucom_signal_ptr_t get_signal(std::string id)</dt>
<dd>Gets a (smart, shared) pointer to the signal whose ID is &ltid&gt</dd>
<dt>int get_signal_index(std::string id)</dt>
<dd>Gets the zero-based index into the signals collection of the signal whose ID is &ltid&gt</dd>
</dl>

### class UcomSignal
#### Description
Represents a UcomSignal. Contains the meta-data required to decode a signal from a data stream

<dl>
<dt>UcomSignal(json signal)</dt>
<dd>Constructs a UcomSignal instance from a JSON representation (usually part of a DBU)</dd>
<dt>UcomSignal(std::string signal_id, UcomSignal::SignalType signal_type)</dt>
<dd>Constructs a UcomSignal instance from the signal ID and signal type</dd>
<dt>std::string get_signal_id()</dt>
<dd>Gets the signal ID</dd>
<dt>const OxTS::Enum::BASIC_TYPE get_data_type()</dt>
<dd>Gets the signal data type</dd>
</dl>

### Python API
### class UcomDbu
#### Description
This class accepts a DBU filename as an argument to its constructor and parses the JSON contained in the DBU to generate collections of UcomMessages and UcomSignals.

<style>
    dt {
        font-weight: bold;
    }
    ul {
        list-style-type: none;
    }
</style>

<dl>
<dt>UcomDbu(filename: str) -> UcomDbu</dt>
<dd>Constructs a UcomDbu instance from a DBU file <filename>. The file must be valid JSON conforming to the DBU schema.</dd>
<dt>get_filename() -> str</dt>
<dd>Gets the filename of the DBU.</dd>
<dt>get_valid() -> bool</dt>
<dd>Gets the 'valid' status of the instance. Returns True if the DBU file was parsed without error; False otherwise.</dd>
<dt>message_id_exists(message_id: int) -> bool</dt>
<dd>Returns True if <message_id> exists in the collection of messages in the DBU; False otherwise.</dd>
<dt>get_messages() -> dict[int, UcomMessage]</dt>
<dd>Gets a dictionary containing all of the UCOM message definitions in the DBU, stored as key : value pairs whose key is the message ID.</dd>
<dt>get_message_ids() -> list[int]</dt>
<dd>Gets a list containing the message IDs of all of the UCOM message definitions in the DBU.</dd>
<dt>get_message(id: int) -> UcomMessage</dt>
<dd>Gets the UcomMessage whose ID is &ltid&gt</dd>
<dt>get_signals(message_id: int) -> list[UcomSignal]</dt>
<dd>Gets a list containing all of the signals in the UcomMessage whose ID is &ltmessage_id&gt.</dd>
<dt>get_data_type(data_type: str) -> OxTS::Enum::BASIC_TYPE</dt>
<dd>Gets the OxTS::Enum::BASIC_TYPE from the string representation of the type.</dd>
</dl>

### class UcomData
#### Description
This class decodes a UCOM packet and allows read-access to the signal values contained within. To decode a UCOM packet, a corresponding valid UcomDbu instance is required to provide the necessary signal layout information.

<dl>
<dt>UcomData(data: str, size: int, dbu: UcomDbu) -> UcomData</dt>
<dd>Constructs a UcomData instance from the byte array passed as the &ltdata&gt argument, whose length in bytes is contained in &ltsize&gt. &ltdbu&gt is a valid UcomDbu instance.</dd>
<dt>peek(data: str, max_size: int) -> tuple[int,</dt>
<dd>Inspect &ltdata&gt to determine if it contains a candidate UCOM packet.<br/>Returns:
<ul>
<li>[-1, False] : if no candidate found</li>
<li>[-1, True]  : if potential candidate found but more data is needed</li>
<li>[packet length, False] : if candidate found</li></dd>
<dt>get_csv() -> str</dt>
<dd>Gets the signal values as a comma-separated string</dd>
<dt>to_string() -> str</dt>
<dd>Gets a string representation of the instance, consisting of message ID, message version and payload length</dd>
<dt>get_message_id() -> int</dt>
<dd>Gets the message ID</dd>
<dt>get_message_version() -> int</dt>
<dd>Gets the message version</dd>
<dt>get_time_frame() -> int</dt>
<dd>Gets the time frame of the 'Arbitrary Time' value</dd>
<dt>get_arbitrary_time() -> int</dt>
<dd>Gets the 'Arbitrary Time' value</dd>
<dt>get_payload_length() -> int</dt>
<dd>Gets the payload length (in bytes)</dd>
<dt>get_signal_count() -> int</dt>
<dd>Gets the signal count</dd>
<dt>get_calc_crc() -> int</dt>
<dd>Gets the calculated CRC</dd>
<dt>get_valid() -> bool</dt>
<dd>Gets the 'valid' status of the signal. Returns True if the UCOM packet has been successfully decoded; False otherwise</dd>
<dt>get(signal_id: str, dbu: UcomDbu) -> tuple[bool, float]</dt>
<dd>Gets the value of the signal whose ID is &ltsignal_id&gt. Returns [True, value] if the signal ID exists and value contains valid data; [False, 0.0] otherwise</dd>
</dl>

### class UcomMessage
#### Description
Represents a UCOM message. Allows read-access to the signals contained within. Derives from *json* class.

<dl>
<dt>UcomMessage(message: json) -> UcomMessage</dt>
<dd>Constructs a UcomMessage instance from a JSON representation (usually part of a DBU)</dd>
<dt>is_valid() -> bool</dt>
<dd>Returns True if the UcomMessage is valid; False otherwise</dd>
<dt>get_id() -> int</dt>
<dd>Gets the ID of the message</dd>
<dt>get_header() -> str</dt>
<dd>Gets a comma-separated string of the names of the signals contained in the message (mainly intended for use when generating CSV output). The string is prepended with 'Time (&lt<i>MessageTiming</i>&gt)', representing the '<i>Arbitrary Time</i>' time frame, e.g. <i>'Time (SDN)'</i></dd>
<dt>get_signal_count() -> int</dt>
<dd>Gets the number of signals contained in the message</dd>
<dt>get_signals() -> list[UcomSignal]</dt>
<dd>Gets a list of signals contained in the message</dd>
<dt>get_signal(id: str) -> UcomSignal</dt>
<dd>Gets the signal whose ID is &ltid&gt</dd>
<dt>get_signal_index(id: str) -> int</dt>
<dd>Gets the zero-based index into the signals collection of the signal whose ID is &ltid&gt</dd>
</dl>

### class UcomSignal
#### Description
Represents a UcomSignal. Contains the meta-data required to decode a signal from a data stream

<dl>
<dt>UcomSignal(signal: json) -> UcomSignal</dt>
<dd>Constructs a UcomSignal instance from a JSON representation (usually part of a DBU)</dd>
<dt>get_signal_id() -> str</dt>
<dd>Gets the signal ID</dd>
<dt>get_data_type() -> OxTS::Enum::BASIC_TYPE</dt>
<dd>Gets the signal data type</dd>
</dl>

## Examples

### C++
#### 1. Parse DBU and display message info

```cpp
#include <iostream>

// Include the header file for UcomDbu
#include "ucom\ucom_dbu.hpp"


int main()
{
    // Parse the DBU
    UcomDbu dbu("..\\..\\oxts.dbu");
    std::cout << (dbu.get_valid() ? "DBU valid" : "DBU invalid") << std::endl;

    // Display the message IDs defined in the DBU
    for (auto id : dbu.get_message_ids())
        std::cout << id << ' ';
    std::cout << std::endl;

    // Display the header of message ID 0
    std::cout << "Header: " << dbu.get_message(0).get_header() << std::endl;

}

```
Generates the following output:

![image](example1.png)

#### 2. Decoding data
```cpp
#include <iostream>
#include "ucom\ucom_dbu.hpp"
#include "ucom\ucom_data.hpp"

const uint8_t _data[] = { 
    0x55,0x4d,0x00,0x00,0x01,0xcc,0xb2,0x9a,0xd0,0x3c,0x00,0x00,0x00,0x00,0xd0,0x00,
    0xc4,0x85,0x05,0x79,0x00,0x00,0x00,0x00,0x34,0x33,0x33,0x33,0x33,0x33,0x26,0x40,
    0x9a,0x99,0x99,0x99,0x99,0x19,0x35,0x40,0x9a,0x99,0x99,0x99,0x99,0x19,0x3f,0x40,
    0xcd,0xcc,0xcc,0xcc,0xcc,0x8c,0x44,0x40,0xcd,0xcc,0xcc,0xcc,0xcc,0x8c,0x49,0x40,
    0xcd,0xcc,0xcc,0xcc,0xcc,0x8c,0x4e,0x40,0x67,0x66,0x66,0x66,0x66,0xc6,0x51,0x40,
    0x66,0x66,0x66,0x66,0x66,0x46,0x54,0x40,0x66,0x66,0x66,0x66,0x66,0xc6,0x56,0x40,
    0x66,0x66,0x66,0x66,0x66,0x46,0x59,0x40,0x66,0x66,0x66,0x66,0x66,0xc6,0x5b,0x40,
    0x66,0x66,0x66,0x66,0x66,0x46,0x5e,0x40,0x33,0x33,0x33,0x33,0x33,0x63,0x60,0x40,
    0x33,0x33,0x33,0x33,0x33,0xa3,0x61,0x40,0x33,0x33,0x33,0x33,0x33,0xe3,0x62,0x40,
    0x33,0x33,0x33,0x33,0x33,0x23,0x64,0x40,0x33,0x33,0x33,0x33,0x33,0x63,0x65,0x40,
    0x33,0x33,0x33,0x33,0x33,0xa3,0x66,0x40,0x33,0x33,0x33,0x33,0x33,0xe3,0x67,0x40,
    0x33,0x33,0x33,0x33,0x33,0x23,0x69,0x40,0x33,0x33,0x33,0x33,0x33,0x63,0x6a,0x40,
    0x33,0x33,0x33,0x33,0x33,0xa3,0x6b,0x40,0x33,0x33,0x33,0x33,0x33,0xe3,0x6c,0x40,
    0x33,0x33,0x33,0x33,0x33,0x23,0x6e,0x40,0x33,0x33,0x33,0x33,0x33,0x63,0x6f,0x40,
    0xb7,0xef,0x80,0xa4, };

int main(int argc, char *argv[])
{
    // Parse the DBU
    UcomDbu dbu("..\\..\\oxts.dbu");

    // Decode the data
    UcomData data(_data, sizeof(_data), dbu);
    std::cout << "UcomData: " << (data.get_valid() ? "valid" : "invalid") << std::endl;
    double value;
    std::cout << "Values: " << std::endl;
    // Iterate over the signal collection and display the values
    for (auto signal : dbu.get_message(data.get_message_id()).get_signals())
    {
        std::string signal_id = signal->get_signal_id();
        if (data.get(signal_id, dbu, value))
            std::cout << signal_id << ": " << value << '\n';
    }
}

```

Generates the following output:

![image](example2.png)

### Python
#### 1. Parse DBU and display message info
```python
from oxts.ucompy import UcomDbu

 # Parse the DBU
dbu = UcomDbu("ucom_decoder/oxts.dbu")
print("DBU valid" if dbu.get_valid() else "DBU invalid")

# Display the message IDs defined in the DBU
for  id in dbu.get_message_ids():
    print(f"{id} ")

# Display the header of message ID 0
print(f"Header: {dbu.get_message(0).get_header()}")
```
Generates the following output:

![image](example3.png)
#### 2. Decoding data
```python
from oxts.ucompy import UcomDbu, UcomData

_data = (
    b'\x55\x4d\x00\x00\x01\xcc\xb2\x9a\xd0\x3c\x00\x00\x00\x00\xd0\x00' + 
    b'\xc4\x85\x05\x79\x00\x00\x00\x00\x34\x33\x33\x33\x33\x33\x26\x40' + 
    b'\x9a\x99\x99\x99\x99\x19\x35\x40\x9a\x99\x99\x99\x99\x19\x3f\x40' + 
    b'\xcd\xcc\xcc\xcc\xcc\x8c\x44\x40\xcd\xcc\xcc\xcc\xcc\x8c\x49\x40' + 
    b'\xcd\xcc\xcc\xcc\xcc\x8c\x4e\x40\x67\x66\x66\x66\x66\xc6\x51\x40' + 
    b'\x66\x66\x66\x66\x66\x46\x54\x40\x66\x66\x66\x66\x66\xc6\x56\x40' + 
    b'\x66\x66\x66\x66\x66\x46\x59\x40\x66\x66\x66\x66\x66\xc6\x5b\x40' + 
    b'\x66\x66\x66\x66\x66\x46\x5e\x40\x33\x33\x33\x33\x33\x63\x60\x40' + 
    b'\x33\x33\x33\x33\x33\xa3\x61\x40\x33\x33\x33\x33\x33\xe3\x62\x40' + 
    b'\x33\x33\x33\x33\x33\x23\x64\x40\x33\x33\x33\x33\x33\x63\x65\x40' + 
    b'\x33\x33\x33\x33\x33\xa3\x66\x40\x33\x33\x33\x33\x33\xe3\x67\x40' + 
    b'\x33\x33\x33\x33\x33\x23\x69\x40\x33\x33\x33\x33\x33\x63\x6a\x40' + 
    b'\x33\x33\x33\x33\x33\xa3\x6b\x40\x33\x33\x33\x33\x33\xe3\x6c\x40' + 
    b'\x33\x33\x33\x33\x33\x23\x6e\x40\x33\x33\x33\x33\x33\x63\x6f\x40' + 
    b'\xb7\xef\x80\xa4')

# Parse the DBU
dbu = UcomDbu("ucom_decoder/oxts.dbu")

# Decode the data
data = UcomData(_data, len(_data), dbu)
print(f"UcomData: {'valid' if data.get_valid() else 'invalid'}")

# double value;
print(f"Signal values in message {data.get_message_id()}: ")
# Iterate over the signal collection and display the values
for signal in dbu.get_message(data.get_message_id()).get_signals():
    signal_id = signal.get_signal_id()
    success, value = data.get(signal_id, dbu)
    print(f"{signal_id} : {value:.2f}")
```

Generates the following output:

![image](example4.png)