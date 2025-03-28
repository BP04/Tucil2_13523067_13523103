cmake_minimum_required(VERSION 3.10)
project(jpeg_test)

set(CMAKE_CXX_STANDARD 17)

add_executable(jpeg_test main.cpp)
target_link_libraries(jpeg_test jpeg)