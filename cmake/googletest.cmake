
include(ExternalProject)

ExternalProject_Add(gtest_ext
    URL                     https://github.com/google/googletest/archive/release-1.8.1.tar.gz
    PREFIX                  googletest
    CMAKE_ARGS              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                            -DBUILD_GMOCK=OFF
                            -DBUILD_SHARED_LIBS=OFF
                            -Dgtest_force_shared_crt=ON
                            -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
                            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                            -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    CMAKE_GENERATOR         ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    INSTALL_COMMAND         ""
)

ExternalProject_Get_Property(gtest_ext SOURCE_DIR BINARY_DIR)

add_library(gtest STATIC IMPORTED GLOBAL)
add_dependencies(gtest gtest_ext)

add_library(gtest_main STATIC IMPORTED GLOBAL)
add_dependencies(gtest_main gtest_ext)

if(MSVC)
    set_target_properties(gtest
        PROPERTIES
        IMPORTED_LOCATION_DEBUG     ${BINARY_DIR}/googletest/gtestd.lib
        IMPORTED_LOCATION           ${BINARY_DIR}/googletest/gtest.lib
    )

    set_target_properties(gtest_main
        PROPERTIES
            IMPORTED_LOCATION_DEBUG     ${BINARY_DIR}/googletest/gtest_maind.lib
            IMPORTED_LOCATION           ${BINARY_DIR}/googletest/gtest_main.lib
    )
else()
    set_target_properties(gtest
        PROPERTIES
        IMPORTED_LOCATION_DEBUG          ${BINARY_DIR}/googletest/libgtestd.a
        IMPORTED_LOCATION_RELEASE        ${BINARY_DIR}/googletest/libgtest.a
        IMPORTED_LOCATION_MINSIZEREL     ${BINARY_DIR}/googletest/libgtest.a
        IMPORTED_LOCATION_RELWITHDEBINFO ${BINARY_DIR}/googletest/libgtest.a
    )

    set_target_properties(gtest_main
        PROPERTIES
            IMPORTED_LOCATION_DEBUG          ${BINARY_DIR}/googletest/libgtest_maind.a
            IMPORTED_LOCATION_RELEASE        ${BINARY_DIR}/googletest/libgtest_main.a
            IMPORTED_LOCATION_MINSIZEREL     ${BINARY_DIR}/googletest/libgtest_main.a
            IMPORTED_LOCATION_RELWITHDEBINFO ${BINARY_DIR}/googletest/libgtest_main.a
    )
endif()

set(GTEST_INCLUDE_DIR ${SOURCE_DIR}/googletest/include)
set(GTEST_LIBRARY gtest gtest_main)

