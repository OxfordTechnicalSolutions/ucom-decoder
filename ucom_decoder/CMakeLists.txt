# CMakeList.txt : CMake project for ucom_decoder, include source and define
# project specific logic here.
#

if(UNIX)
    option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
endif()

message("BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS})
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
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
   json/include
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ucom_decoder PROPERTY CXX_STANDARD 20)
endif()

message("CMAKE_INSTALL_LIBDIR: " ${CMAKE_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR})
message("CMAKE_INSTALL_BINDIR: " ${CMAKE_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR})
install(
    TARGETS ucom_decoder
    EXPORT ucom_decoder
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

# TODO: Add tests and install targets if needed.
