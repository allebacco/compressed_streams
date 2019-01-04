
include(ExternalProject)

ExternalProject_Add(zlib
    GIT_REPOSITORY  https://github.com/madler/zlib.git
    GIT_TAG         v1.2.11
    CMAKE_ARGS      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    INSTALL_COMMAND   ""
    BUILD_IN_SOURCE   1
)

set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/zlib-prefix/src/zlib)
set(ZLIB_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/zlib-prefix/src/zlib)
set(ZLIB_LIBRARY ${ZLIB_LIB_DIR}/libz.a)
