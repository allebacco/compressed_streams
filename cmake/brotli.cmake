
include(ExternalProject)

ExternalProject_Add(brotli_ext
    URL                 https://github.com/google/brotli/archive/v1.0.7.tar.gz
    PREFIX              brotli
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

ExternalProject_Get_Property(brotli_ext SOURCE_DIR BINARY_DIR)

add_library(brotli_common STATIC IMPORTED GLOBAL)
add_library(brotli_dec STATIC IMPORTED GLOBAL)
add_library(brotli_enc STATIC IMPORTED GLOBAL)

add_dependencies(brotli_common brotli_ext)
add_dependencies(brotli_dec brotli_ext)
add_dependencies(brotli_enc brotli_ext)

if(MSVC)
    # TODO Add windows support
else()
    set_target_properties(brotli_common
        PROPERTIES
            IMPORTED_LOCATION ${BINARY_DIR}/libbrotlicommon-static.a
    )
    set_target_properties(brotli_dec
        PROPERTIES
            IMPORTED_LOCATION ${BINARY_DIR}/libbrotlidec-static.a
    )
    set_target_properties(brotli_enc
        PROPERTIES
            IMPORTED_LOCATION ${BINARY_DIR}/libbrotlienc-static.a
    )
endif()

set(BROTLI_INCLUDE_DIR ${SOURCE_DIR}/c/include)
set(BROTLI_LIBRARY brotli_common brotli_dec brotli_enc)
