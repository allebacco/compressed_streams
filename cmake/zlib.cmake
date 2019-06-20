
include(ExternalProject)

ExternalProject_Add(zlib_ext
    URL                 http://zlib.net/zlib-1.2.11.tar.gz
    PREFIX              zlib
    CMAKE_ARGS          -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                        -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
                        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    CMAKE_GENERATOR     ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    INSTALL_COMMAND     ""
)

ExternalProject_Get_Property(zlib_ext SOURCE_DIR BINARY_DIR)

add_library(zlib STATIC IMPORTED GLOBAL)
add_dependencies(zlib zlib_ext)

if(MSVC)
    set_target_properties(zlib
        PROPERTIES
            IMPORTED_LOCATION_DEBUG     ${BINARY_DIR}/zlibstaticd.lib
            IMPORTED_LOCATION           ${BINARY_DIR}/zlibstatic.lib
    )
else()
    set_target_properties(zlib
        PROPERTIES
            IMPORTED_LOCATION   ${BINARY_DIR}/libz.a
    )
endif()

set(ZLIB_INCLUDE_DIR ${SOURCE_DIR} ${BINARY_DIR})
set(ZLIB_LIBRARY zlib)

