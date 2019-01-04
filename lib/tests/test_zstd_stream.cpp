

#include <algorithm>
#include <numeric>
#include <vector>
#include <sstream>
#include <strstream>
#include <gtest/gtest.h>
#include <zstd.h>

#include "../include/compressed_streams/zstd_stream.h"


std::vector<char> generate_data_range(const size_t count)
{
    std::vector<char> data(count);
    for(size_t i=0; i<count; ++i)
        data[i] = static_cast<char>(i & 0xFF);

    return data;
}


TEST(ZstdStream, Compression)
{
    std::vector<char> data = generate_data_range(10);

    std::ostringstream ostream(std::ios_base::out|std::ios_base::binary);
    {
    compressed_streams::ZstdOStream zstd_ostream(ostream, 1);
    zstd_ostream.write(data.data(), data.size());
    }

    std::string result = ostream.str();

    std::vector<char> test_data(data.size());
    size_t decompressed_bytes = ZSTD_decompress(test_data.data(), test_data.size(), result.data(), result.size());

    EXPECT_EQ(decompressed_bytes, data.size());
    EXPECT_EQ(data, test_data);
}


TEST(ZstdStream, Decompress)
{
    std::vector<char> data = generate_data_range(10);

    std::vector<char> compressed_data(data.size() + 5000);
    size_t compressed_bytes = ZSTD_compress(compressed_data.data(), compressed_data.size(), data.data(), data.size(), ZSTD_CLEVEL_DEFAULT);

    std::vector<char> test_data(data.size());
    std::istrstream istream(compressed_data.data(), compressed_bytes);
    {
    compressed_streams::ZstdIStream zstd_istream(istream);
    zstd_istream.read(test_data.data(), test_data.size());
    std::streamsize read_count = zstd_istream.gcount();

    EXPECT_EQ(data.size(), read_count);

    EXPECT_EQ(data, test_data);
    }
}

