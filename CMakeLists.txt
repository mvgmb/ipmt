cmake_minimum_required(VERSION 3.15)
project(ipmt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -O2)
set(EXECUTABLE_OUTPUT_PATH ./bin)

add_executable(ipmt src/main.cpp src/lz77.cpp src/lz77.h src/suffix_array.cpp src/suffix_array.h)