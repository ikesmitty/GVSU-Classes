cmake_minimum_required(VERSION 3.6)
project(program2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    program2.c
    savethis.c
    sort.c)

add_executable(program2 ${SOURCE_FILES})