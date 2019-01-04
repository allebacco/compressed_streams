


set(SOURCES
    src/zstd_stream.cpp
    src/zlib_stream.cpp
    src/internal/strict_fstream.hpp
    src/internal/zstr.hpp
)

set(HEADERS
    include/compressed_streams/zstd_stream.h
    include/compressed_streams/zlib_stream.h
    include/compressed_streams/istreambuf_view.h
)


add_library(compressed_streams STATIC ${HEADERS} ${SOURCES})
target_link_libraries(compressed_streams ${ZSTD_LIBRARY} ${ZLIB_LIBRARY})
add_dependencies(compressed_streams libzstd zlib)


set(TEST_SOURCES
    tests/test_zstd_stream.cpp
    tests/test_zlib_stream.cpp
)

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(NAME ${TEST_FILE} NAME_WE)

    add_executable(${NAME} ${TEST_FILE})
    target_link_libraries(${NAME} gtest_main compressed_streams)
    add_test(NAME ${NAME} COMMAND ${NAME})
endforeach()

#add_executable(test_zstd_stream tests/test_zstd_stream.cpp)
#target_link_libraries(test_zstd_stream gtest_main compressed_streams)
#add_test(NAME test_zstd_stream COMMAND test_zstd_stream)
