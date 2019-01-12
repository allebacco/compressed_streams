

#include <algorithm>
#include <numeric>
#include <vector>
#include <sstream>
#include <strstream>
#include <gtest/gtest.h>

#include "../include/compressed_streams/lz4_stream.h"


std::vector<char> generate_data_range(const size_t count)
{
    std::vector<char> data(count);
    for(size_t i=0; i<count; ++i)
        data[i] = static_cast<char>(i & 0xFF);

    return data;
}


TEST(Lz4Stream, Compression_Decompression)
{
    std::vector<char> data = generate_data_range(10);

    std::ostringstream ostream(std::ios_base::out|std::ios_base::binary);
    {
    compressed_streams::Lz4OStream lz4_ostream(ostream);
    lz4_ostream.write(data.data(), data.size());
    }

    std::vector<char> test_data(data.size());
    std::istringstream istream(ostream.str(), std::ios_base::in|std::ios_base::binary);
    {
    compressed_streams::Lz4IStream lz4_istream(istream);
    lz4_istream.read(test_data.data(), test_data.size());

    EXPECT_EQ(data.size(), lz4_istream.gcount());
    }

    EXPECT_EQ(data, test_data);
}
