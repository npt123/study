cmake_minimum_required(VERSION 3.8)
project(pthread)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c server.c client.c server_transfer_file.c server_send_info.c)
add_executable(pthread ${SOURCE_FILES})