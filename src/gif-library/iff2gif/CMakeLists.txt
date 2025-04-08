cmake_minimum_required(VERSION 3.16)
project(iff2gif)

set(IFF2GIF_SOURCES
    gifwrite.cpp
    iffread.cpp
    chunky.cpp
    planar.cpp
    ppunpack.cpp
    rotate.cpp
    mediancut.cpp
    quantizer.cpp
    palette.cpp
)

add_library(iff2gif_lib STATIC ${IFF2GIF_SOURCES})

target_include_directories(iff2gif_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET iff2gif_lib PROPERTY CXX_STANDARD 17)
