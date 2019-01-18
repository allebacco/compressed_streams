
include(ExternalProject)

ExternalProject_Add(gtest_ext
    GIT_REPOSITORY          https://github.com/google/googletest.git
    GIT_TAG                 master
    PREFIX                  googletest
    CMAKE_ARGS              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                            -DBUILD_GMOCK=OFF
                            -DBUILD_SHARED_LIBS=OFF
                            -Dgtest_force_shared_crt=ON
                            -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
    CMAKE_GENERATOR         ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    UPDATE_DISCONNECTED     ON
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
        IMPORTED_LOCATION_DEBUG          ${BINARY_DIR}/lib/Debug/gtestd.lib
        IMPORTED_LOCATION_RELEASE        ${BINARY_DIR}/lib/Release/gtest.lib
        IMPORTED_LOCATION_MINSIZEREL     ${BINARY_DIR}/lib/MinSizeRel/gtest.lib
        IMPORTED_LOCATION_RELWITHDEBINFO ${BINARY_DIR}/lib/RelWithDebInfo/gtest.lib
    )

    set_target_properties(gtest_main
        PROPERTIES
            IMPORTED_LOCATION_DEBUG          ${BINARY_DIR}/lib/Debug/gtest_maind.lib
            IMPORTED_LOCATION_RELEASE        ${BINARY_DIR}/lib/Release/gtest_main.lib
            IMPORTED_LOCATION_MINSIZEREL     ${BINARY_DIR}/lib/MinSizeRel/gtest_main.lib
            IMPORTED_LOCATION_RELWITHDEBINFO ${BINARY_DIR}/lib/RelWithDebInfo/gtest_main.lib
    )
else()
    set_target_properties(gtest
        PROPERTIES
        IMPORTED_LOCATION_DEBUG          ${BINARY_DIR}/lib/libgtestd.a
        IMPORTED_LOCATION_RELEASE        ${BINARY_DIR}/lib/libgtest.a
        IMPORTED_LOCATION_MINSIZEREL     ${BINARY_DIR}/lib/libgtest.a
        IMPORTED_LOCATION_RELWITHDEBINFO ${BINARY_DIR}/lib/libgtest.a
    )

    set_target_properties(gtest_main
        PROPERTIES
            IMPORTED_LOCATION_DEBUG          ${BINARY_DIR}/lib/libgtest_maind.a
            IMPORTED_LOCATION_RELEASE        ${BINARY_DIR}/lib/libgtest_main.a
            IMPORTED_LOCATION_MINSIZEREL     ${BINARY_DIR}/lib/libgtest_main.a
            IMPORTED_LOCATION_RELWITHDEBINFO ${BINARY_DIR}/lib/libgtest_main.a
    )
endif()

set(GTEST_INCLUDE_DIR ${SOURCE_DIR}/googletest/include)
set(GTEST_LIBRARY gtest gtest_main)

