cmake_minimum_required(VERSION 3.10.0)
project(ndm-server VERSION 1.2.0 LANGUAGES C CXX)

add_executable(ndm-server main.cpp server.cpp)

#include(CTest)
#enable_testing()

