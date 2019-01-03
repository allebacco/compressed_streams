cmake_minimum_required(VERSION 3.5)
project(compressed_streams CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

include_directories(
    /home/alle/Sources/zstd/lib
)
link_directories(
    /home/alle/Sources/zstd/build/cmake/build/lib
)

include(cmake/googletest.cmake)
include(cmake/zstd.cmake)
enable_testing()


include_directories(${ZSTD_INCLUDE_DIR})
link_directories(${ZSTD_LIB_DIR})

add_subdirectory(lib)
