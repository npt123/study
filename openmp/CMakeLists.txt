cmake_minimum_required(VERSION 3.9)
project(openmp C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

add_executable(openmp main.c)