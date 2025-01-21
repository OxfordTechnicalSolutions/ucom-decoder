# CMakeList.txt : CMake project for ucom_decoder, include source and define
# project specific logic here.
#


# Add source to the UCOM to csv executable.
add_executable (ucom_to_csv 
   "ucom_decoder.cpp" 
   "args.cpp" 
   "socket_helper.cpp" 
   "input_file.cpp" 
   "application.cpp" 
   "ucom_decoder_app.cpp" 
   "duration.cpp" 
   "quitter.cpp" 
   "cmd_line_quitter.cpp"
)

add_dependencies(ucom_to_csv ucom_decoder)

# libraries for winsock
if(WIN32)
	target_link_libraries(ucom_to_csv wsock32 ws2_32 ucom_decoder)
endif()

if(UNIX)
   find_package(Threads REQUIRED)
   target_link_libraries(ucom_to_csv PRIVATE Threads::Threads ucom_decoder)
endif()

add_custom_command(TARGET ucom_to_csv POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/oxts.dbu
                       ${CMAKE_CURRENT_BINARY_DIR}/oxts.dbu)

add_custom_command(TARGET ucom_to_csv POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/example_data/241120_SN62084.ucom
                       ${CMAKE_CURRENT_BINARY_DIR}/input.ucom)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ucom_decoder PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
