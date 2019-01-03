
cmake_minimum_required(VERSION 3.5)

include(ExternalProject)

ExternalProject_Add(libzstd
    GIT_REPOSITORY    https://github.com/facebook/zstd.git
    GIT_TAG           master
    SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/libzstd-src/build/cmake"
    BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/libzstd-build"
    CMAKE_ARGS        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DZSTD_BUILD_PROGRAMS=OFF -DZSTD_BUILD_SHARED=OFF
    INSTALL_COMMAND   ""
)

set(ZSTD_LIBRARY zstd)
set(ZSTD_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/libzstd-build/lib)
set(ZSTD_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libzstd-src/lib)
