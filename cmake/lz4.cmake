
include(ExternalProject)

ExternalProject_Add(liblz4
    GIT_REPOSITORY    https://github.com/lz4/lz4.git
    GIT_TAG           master
    SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/liblz4-prefix/src/liblz4/contrib/cmake_unofficial"
    CMAKE_ARGS        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DLZ4_BUILD_LEGACY_LZ4C=OFF
                      -DLZ4_BUNDLED_MODE=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON
    INSTALL_COMMAND   ""
)

set(LZ4_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/liblz4-prefix/src/liblz4-build)
set(LZ4_LIBRARY ${LZ4_LIB_DIR}/liblz4.a)
set(LZ4_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/liblz4-prefix/src/liblz4/lib)
