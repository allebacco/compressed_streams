


set(SOURCES
    src/zstd_stream.cpp
)

set(HEADERS
    include/compressed_streams/zstd_stream.h
    include/compressed_streams/istreambuf_view.h
)


add_library(compressed_streams STATIC ${HEADERS} ${SOURCES})
target_link_libraries(compressed_streams ${ZSTD_LIBRARY})


# Now simply link against gtest or gtest_main as needed. Eg
add_executable(test_zstd_stream tests/test_zstd_stream.cpp)
target_link_libraries(test_zstd_stream gtest_main compressed_streams)
add_test(NAME test_zstd_stream COMMAND test_zstd_stream)
