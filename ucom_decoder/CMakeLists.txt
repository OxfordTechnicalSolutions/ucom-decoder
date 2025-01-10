# CMakeList.txt : CMake project for ucom_decoder, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (ucom_decoder "ucom_decoder.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ucom_decoder PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
