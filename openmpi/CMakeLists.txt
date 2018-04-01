cmake_minimum_required(VERSION 3.9)
project(openmpi)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER mpicxx)

add_executable(openmpi main.cpp)