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