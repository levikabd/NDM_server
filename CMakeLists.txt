cmake_minimum_required(VERSION 3.10.0)
project(ndm_server VERSION 0.1.0 LANGUAGES C CXX)

add_executable(ndm_server main.cpp server.cpp)

include(CTest)
enable_testing()

