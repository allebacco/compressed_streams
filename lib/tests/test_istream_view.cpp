#include <algorithm>
#include <numeric>
#include <vector>
#include <sstream>
#include <strstream>
#include <gtest/gtest.h>

#include "../include/compressed_streams/istream_view.h"

using namespace compressed_streams;

std::vector<char> generate_data_range(const size_t count)
{
    std::vector<char> data(count);
    for(size_t i=0; i<count; ++i)
        data[i] = static_cast<char>(i & 0xFF);

    return data;
}


TEST(IStreamView, ReadFromBuffer)
{
    std::vector<char> data = generate_data_range(200);

    IStreamView istream(data.data(), data.size());

    std::vector<char> test_data;
    for(size_t i=0; i<data.size(); ++i)
        test_data.push_back(istream.get());

    EXPECT_EQ(data, test_data);
}
