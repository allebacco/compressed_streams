
include(ExternalProject)

ExternalProject_Add(snappy_ext
    URL                 https://github.com/google/snappy/archive/1.1.7.tar.gz
    PREFIX              snappy
    CMAKE_ARGS          -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                        -DBUILD_SHARED_LIBS=OFF
                        -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
                        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                        -DSNAPPY_BUILD_TESTS=OFF
    CMAKE_GENERATOR     ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    INSTALL_COMMAND     ""
)

ExternalProject_Get_Property(snappy_ext SOURCE_DIR BINARY_DIR)

add_library(snappy STATIC IMPORTED GLOBAL)
add_dependencies(snappy snappy_ext)

if(MSVC)
    set_target_properties(snappy
        PROPERTIES
            IMPORTED_LOCATION ${BINARY_DIR}/snappy.lib
    )
else()
    set_target_properties(snappy
        PROPERTIES
            IMPORTED_LOCATION ${BINARY_DIR}/libsnappy.a
    )
endif()

set(SNAPPY_INCLUDE_DIR ${SOURCE_DIR} ${BINARY_DIR})
set(SNAPPY_LIBRARY snappy)

