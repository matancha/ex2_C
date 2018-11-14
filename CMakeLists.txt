cmake_minimum_required(VERSION 3.12)
project(ex2 C)

set(CMAKE_C_STANDARD 99)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wvla -std=c99")

add_executable(ex2 CompareSequences.c)