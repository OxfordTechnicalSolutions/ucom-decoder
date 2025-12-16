To run all of the automated tests, first build ucom_to_csv and then from the ucom_decoder/test/ folder, run:

run_tests.bat (Windows)

run_tests.sh (Linux)

The automated unit tests run on the Python bindings to simulataneously test the bindings and the underlying (bound) C++ code.

There is also a test of the overall (C++) decoder functionality using automatically-generated UCOM data. A range of known values are encoded into UCOM packets. These packets are then decoded (using ucom_to_csv) and the extracted values are compared with the original values.

The Python unit tests can be run on their own by changing to the ucom_decoder_py/tests folder and running:

run_tests.bat (Windows)

run_tests.sh (Linux)