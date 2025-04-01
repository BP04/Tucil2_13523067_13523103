cmake_minimum_required(VERSION 3.10)
project(ImageCompressor)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCES
    src/ImageProcessing.cpp
    src/Metrics.cpp
    src/QuadTree.cpp
)

add_executable(prog ${SOURCES})