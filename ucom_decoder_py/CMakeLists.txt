cmake_minimum_required(VERSION 3.8)

project(ucom_sdk)

add_subdirectory(pybind11)

pybind11_add_module(ucom_sdk ucom_py_bindings.cpp)

target_link_libraries(ucom_sdk PUBLIC ucom_decoder)