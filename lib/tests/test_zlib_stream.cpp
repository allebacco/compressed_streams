

#include <algorithm>
#include <numeric>
#include <vector>
#include <sstream>
#include <strstream>
#include <gtest/gtest.h>

#include "../include/compressed_streams/zlib_stream.h"


std::vector<char> generate_data_range(const size_t count)
{
    std::vector<char> data(count);
    for(size_t i=0; i<count; ++i)
        data[i] = static_cast<char>(i & 0xFF);

    return data;
}


TEST(ZstdStream, Compression_Decompression)
{
    std::vector<char> data = generate_data_range(10);

    std::ostringstream ostream(std::ios_base::out|std::ios_base::binary);
    {
    compressed_streams::ZlibOStream zstd_ostream(ostream);
    zstd_ostream.write(data.data(), data.size());
    }

    std::vector<char> test_data(data.size());
    std::istringstream istream(ostream.str(), std::ios_base::in|std::ios_base::binary);
    {
    compressed_streams::ZlibIStream zstd_istream(istream);
    zstd_istream.read(test_data.data(), test_data.size());
    std::streamsize read_count = zstd_istream.gcount();

    EXPECT_EQ(data.size(), read_count);

    EXPECT_EQ(data, test_data);
    }
}
