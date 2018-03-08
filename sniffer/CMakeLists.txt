cmake_minimum_required(VERSION 3.8)
project(sniffer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        sniffer.c)

add_executable(sniffer ${SOURCE_FILES})