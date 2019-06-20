
find_package(Threads REQUIRED)


set(SOURCES
    src/zstd_stream.cpp
    src/zlib_stream.cpp
    src/lz4_stream.cpp
    src/istream_view.cpp
    src/byte_ostream.cpp
    src/lzma_stream.cpp
    src/brotli_stream.cpp
    src/internal/strict_fstream.hpp
    src/internal/zstr.hpp
)

set(HEADERS
    include/compressed_streams/zstd_stream.h
    include/compressed_streams/zlib_stream.h
    include/compressed_streams/lz4_stream.h
    include/compressed_streams/istream_view.h
    include/compressed_streams/byte_ostream.h
    include/compressed_streams/lzma_stream.h
    include/compressed_streams/brotli_stream.h
)


add_library(compressed_streams STATIC ${HEADERS} ${SOURCES})
target_link_libraries(compressed_streams
    PUBLIC
        ${ZSTD_LIBRARY} ${ZLIB_LIBRARY} ${LZ4_LIBRARY} Threads::Threads lzma ${BROTLI_LIBRARY} ${SNAPPY_LIBRARY}
)

target_include_directories(compressed_streams
    PRIVATE 
        ${ZSTD_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${LZ4_INCLUDE_DIR} ${BROTLI_INCLUDE_DIR} ${SNAPPY_INCLUDE_DIR}
)

if(WIN32)
    set_target_properties(compressed_streams 
        PROPERTIES
            LINK_FLAGS "/WHOLEARCHIVE"
    )
else()
    set_target_properties(compressed_streams 
        PROPERTIES
            LINK_FLAGS "-Wl,--whole-archive"
    )
endif()


set(TEST_SOURCES
    tests/test_zstd_stream.cpp
    tests/test_zlib_stream.cpp
    tests/test_lz4_stream.cpp
    tests/test_istream_view.cpp
    tests/test_lzma_stream.cpp
    tests/test_brotli_stream.cpp
)

set(TEST_COMMON
    tests/common.h
)

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(NAME ${TEST_FILE} NAME_WE)

    add_executable(${NAME} ${TEST_FILE} ${TEST_COMMON})
    target_link_libraries(${NAME} PRIVATE ${GTEST_LIBRARY} compressed_streams Threads::Threads)
    target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${GTEST_INCLUDE_DIR})
    add_test(NAME ${NAME} COMMAND ${NAME})
endforeach()
