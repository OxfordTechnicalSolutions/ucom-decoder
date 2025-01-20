# ucom_decoder

## Clone the repository
```sh
git clone git@github.com:OxfordTechnicalSolutions/UCOM_decoder.git
```
## Initialise sub-module
UCOM decoder uses a sub-module for the JSON parser. Initialise the sub-module:

```sh
git submodule update --init
```

## Visual Studio
The project is set up to allow cross-platform build (Windows / Linux) using Visual Studio 2022. The Linux build would most commonly use WSL2, but it is possible to use other remote hosts.

To start, open the project folder in Visual Studio. VS should automatically detect the CMake files and make the configurations available in the menu.

### Quick Start
Select the host, e.g.:
Local Machine for Windows build
WSL: Ubuntu for Linux build

Select the build configuration, e.g.:
x64 Debug

Select the 'Startup Item', e.g.:
ucom_decoder.exe

Run the selected configuration

## UCOM decoder command-line options

Running UCOM decoder with no arguments produces the following output:

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