cmake_minimum_required(VERSION 3.15...3.26)

SET(SKBUILD_PROJECT_NAME ucompy)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(PYBIND11_NEWPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(${SKBUILD_PROJECT_NAME} ucom_py_bindings.cpp)
target_include_directories(${SKBUILD_PROJECT_NAME} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../ucom_decoder/include 
	${CMAKE_CURRENT_SOURCE_DIR}/../ucom_decoder/json/include
)

target_link_libraries(${SKBUILD_PROJECT_NAME} PRIVATE ucom_decoder)

SET(CMAKE_INSTALL_LIBDIR lib)

add_subdirectory(../ucom_decoder library)

install(TARGETS ${SKBUILD_PROJECT_NAME} LIBRARY DESTINATION oxts)