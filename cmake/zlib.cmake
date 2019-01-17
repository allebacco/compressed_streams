
include(ExternalProject)

ExternalProject_Add(zlib_ext
    GIT_REPOSITORY  https://github.com/madler/zlib.git
    GIT_TAG         v1.2.11
    PREFIX          zlib
    CMAKE_ARGS      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    CMAKE_GENERATOR          ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    UPDATE_DISCONNECTED     ON
    INSTALL_COMMAND   ""
)

ExternalProject_Get_Property(zlib_ext SOURCE_DIR BINARY_DIR)

add_library(zlib STATIC IMPORTED GLOBAL)
add_dependencies(zlib zlib_ext)

if(MSVC)
    set_target_properties(zlib
        PROPERTIES
            IMPORTED_LOCATION_DEBUG             ${BINARY_DIR}/Debug/zlibstaticd.lib
            IMPORTED_LOCATION_RELEASE           ${BINARY_DIR}/Release/zlibstatic.lib
            IMPORTED_LOCATION_MINSIZEREL        ${BINARY_DIR}/MinSizeRel/zlibstatic.lib
            IMPORTED_LOCATION_RELWITHDEBINFO    ${BINARY_DIR}/RelWithDebInfo/zlibstatic.lib
    )
else()
    set_target_properties(zlib
        PROPERTIES
            IMPORTED_LOCATION             ${BINARY_DIR}/libz.a
    )
endif()

set(ZLIB_LIBRARY zlib)
set(ZLIB_INCLUDE_DIR ${SOURCE_DIR} ${BINARY_DIR})
