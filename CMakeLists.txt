# source

cmake_minimum_required(VERSION 3.20)

project(CMatrix LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

add_library(CMatrix SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/cmatrix.c)
target_include_directories(CMatrix PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(program/multiplication)
add_subdirectory(program/row-echelon)
