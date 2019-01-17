
include(ExternalProject)

ExternalProject_Add(zstd_ext
    GIT_REPOSITORY    https://github.com/facebook/zstd.git
    GIT_TAG           master
    PREFIX            zstd
    CMAKE_ARGS        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                      -DZSTD_BUILD_PROGRAMS=OFF 
                      -DZSTD_BUILD_SHARED=OFF 
                      -DZSTD_LEGACY_SUPPORT=OFF
    CMAKE_GENERATOR   ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    PATCH_COMMAND     ${CMAKE_COMMAND} 
                          -E copy ${CMAKE_SOURCE_DIR}/cmake/patch/zstd_CMakeLists.txt CMakeLists.txt
    UPDATE_DISCONNECTED     ON
    INSTALL_COMMAND   ""
)

ExternalProject_Get_Property(zstd_ext SOURCE_DIR BINARY_DIR)

add_library(zstd STATIC IMPORTED GLOBAL)
add_dependencies(zstd zstd_ext)

if(MSVC)
    set_target_properties(zstd
        PROPERTIES
            IMPORTED_LOCATION_DEBUG             ${BINARY_DIR}/build/cmake/lib/Debug/zstd_static.lib
            IMPORTED_LOCATION_RELEASE           ${BINARY_DIR}/build/cmake/lib/Release/zstd_static.lib
            IMPORTED_LOCATION_MINSIZEREL        ${BINARY_DIR}/build/cmake/lib/MinSizeRel/zstd_static.lib
            IMPORTED_LOCATION_RELWITHDEBINFO    ${BINARY_DIR}/build/cmake/lib/RelWithDebInfo/zstd_static.lib
    )
else()
    set_target_properties(zstd
        PROPERTIES
            IMPORTED_LOCATION ${BINARY_DIR}/build/cmake/lib/libzstd.a
    )
endif()

set(ZSTD_LIBRARY zstd)
set(ZSTD_INCLUDE_DIR ${SOURCE_DIR}/lib)