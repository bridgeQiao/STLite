cmake_minimum_required(VERSION 3.0.0)
project(toySTL VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
include_directories(src/stlite)

add_executable(toySTL main.cpp)

add_subdirectory(tests)
