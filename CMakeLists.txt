cmake_minimum_required(VERSION 3.10)
project(ImageCompressor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(src/gif-library/iff2gif)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/gif-library/iff2gif
)

set(SOURCES
    src/ImageProcessing.cpp
    src/Metrics.cpp
    src/QuadTree.cpp
    src/gifenc.c
)

add_executable(runner ${SOURCES})
target_link_libraries(runner PRIVATE iff2gif_lib)
