cmake_minimum_required(VERSION 3.5)
project(compressed_streams C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(cmake/googletest.cmake)
include(cmake/zstd.cmake)
include(cmake/zlib.cmake)
include(cmake/lz4.cmake)

enable_testing()

add_subdirectory(external/liblzma)
add_subdirectory(lib)
