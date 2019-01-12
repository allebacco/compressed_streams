
include(ExternalProject)

ExternalProject_Add(lz4_ext
    GIT_REPOSITORY    https://github.com/lz4/lz4.git
    GIT_TAG           master
    PREFIX            lz4
    CMAKE_ARGS        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DLZ4_BUILD_LEGACY_LZ4C=OFF
                      -DLZ4_BUNDLED_MODE=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON
    CMAKE_GENERATOR   ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    PATCH_COMMAND     ${CMAKE_COMMAND} 
                        -E copy ${CMAKE_SOURCE_DIR}/cmake/patch/lz4_CMakeLists.txt CMakeLists.txt
    UPDATE_DISCONNECTED     ON
    INSTALL_COMMAND   ""
)


ExternalProject_Get_Property(lz4_ext SOURCE_DIR BINARY_DIR)

add_library(lz4 STATIC IMPORTED GLOBAL)
add_dependencies(lz4 lz4_ext)
set_target_properties(lz4
    PROPERTIES
        IMPORTED_LOCATION ${BINARY_DIR}/contrib/cmake_unofficial/liblz4.a
)

set(LZ4_LIBRARY lz4)
set(LZ4_INCLUDE_DIR ${SOURCE_DIR}/lib)


