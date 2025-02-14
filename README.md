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
NB Use the appropriate command (instead of python) to invoke the Python interpreter on you system, e.g. py, python, python3. 
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
