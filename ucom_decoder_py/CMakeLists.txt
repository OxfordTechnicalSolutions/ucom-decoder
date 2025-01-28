cmake_minimum_required(VERSION 3.15...3.26)

SET(SKBUILD_PROJECT_NAME oxts)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(PYBIND11_NEWPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(ucompy ucom_py_bindings.cpp)
target_include_directories(ucompy PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../ucom_decoder/include 
	${CMAKE_CURRENT_SOURCE_DIR}/../ucom_decoder/json/include
)

target_link_libraries(ucompy PRIVATE ucom_decoder)

add_subdirectory(../ucom_decoder library)

install(TARGETS ucompy LIBRARY DESTINATION ${SKBUILD_PROJECT_NAME})