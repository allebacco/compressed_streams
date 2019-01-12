

#include "../include/compressed_streams/zlib_stream.h"

#include "common.h"

using namespace compressed_streams;

using ZlibIOStreamTypes = ::testing::Types<std::tuple<ZlibOStream, ZlibIStream>>;
INSTANTIATE_TYPED_TEST_SUITE_P(ZlibIOStreamTest, IOStreamTest, ZlibIOStreamTypes);
