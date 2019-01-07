
include(ExternalProject)

ExternalProject_Add(libzstd
    GIT_REPOSITORY    https://github.com/facebook/zstd.git
    GIT_TAG           master
    SOURCE_DIR        ${CMAKE_CURRENT_BINARY_DIR}/libzstd-prefix/src/libzstd/build/cmake
    CMAKE_ARGS        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DZSTD_BUILD_PROGRAMS=OFF -DZSTD_BUILD_SHARED=OFF -DZSTD_LEGACY_SUPPORT=OFF
    INSTALL_COMMAND   ""
)

set(ZSTD_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/libzstd-prefix/src/libzstd-build/lib)
set(ZSTD_LIBRARY ${ZSTD_LIB_DIR}/libzstd.a)
set(ZSTD_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libzstd-prefix/src/libzstd/lib)
