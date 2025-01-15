# CMakeList.txt : CMake project for ucom_decoder, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (ucom_decoder "ucom_decoder.cpp" "args.cpp" "ucom_message.cpp" "ucom_signal.cpp" "ucom_dbu.cpp" "crc.cpp" "socket_helper.cpp" "input_file.cpp" "application.cpp" "ucom_decoder_app.cpp" "ucom_data.cpp")

# include directory for json parser
include_directories(json/include)

# libraries for winsock
if(WIN32)
	target_link_libraries(ucom_decoder wsock32 ws2_32)
endif()

add_custom_command(TARGET ucom_decoder POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/oxts.dbu
                       ${CMAKE_CURRENT_BINARY_DIR}/oxts.dbu)

add_custom_command(TARGET ucom_decoder POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/UCOM_241120_SN62084/241120_SN62084_1.ucom
                       ${CMAKE_CURRENT_BINARY_DIR}/input.ucom)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ucom_decoder PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
