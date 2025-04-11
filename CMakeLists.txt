cmake_minimum_required(VERSION 3.10)
project(ImageCompressor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(OUTPUT_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)

if (MINGW OR WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libstdc++ -static-libgcc")
endif()

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(src/gif-library/iff2gif)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/gif-library/iff2gif
)

set(SOURCES
    src/ImageProcessing.cpp
    src/Metrics.cpp
    src/QuadTree.cpp
    src/gifenc.c
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(EXECUTABLE_NAME runner-win)
else()
    set(EXECUTABLE_NAME runner)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_DIR})

add_executable(${EXECUTABLE_NAME} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE iff2gif_lib)
