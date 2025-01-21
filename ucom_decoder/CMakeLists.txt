# CMakeList.txt : CMake project for ucom_decoder, include source and define
# project specific logic here.
#


# Add source to the UCOM decoder library.
add_library (ucom_decoder 
   "src/ucom_message.cpp" 
   "src/ucom_signal.cpp" 
   "src/ucom_dbu.cpp" 
   "src/crc.cpp" 
   "src/ucom_data.cpp"
)

target_include_directories(ucom_decoder PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   json/include
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ucom_decoder PROPERTY CXX_STANDARD 20)
endif()

#install(TARGETS ucom_decoder FILE_SET HEADERS)

# TODO: Add tests and install targets if needed.
