

#include <algorithm>
#include <numeric>
#include <vector>
#include <sstream>
#include <strstream>
#include <gtest/gtest.h>

#include "../include/compressed_streams/lz4_stream.h"

#include "common.h"

using namespace compressed_streams;

using Lz4IOStreamTypes = ::testing::Types<std::tuple<Lz4OStream, Lz4IStream>>;
INSTANTIATE_TYPED_TEST_SUITE_P(Lz4IOStreamTest, IOStreamTest, Lz4IOStreamTypes);


