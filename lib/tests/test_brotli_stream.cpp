
#include "common.h"

#include "../include/compressed_streams/brotli_stream.h"

using namespace compressed_streams;


using BrotliIOStreamTypes = ::testing::Types<std::tuple<BrotliOStream, BrotliIStream>>;
INSTANTIATE_TYPED_TEST_CASE_P(BrotliIOStreamTest, IOStreamTest, BrotliIOStreamTypes);



TEST(Brotli, Compression_Decompression)
{
    std::vector<char> ref_compression = BrotliOStream::compress(DATA_REF);
    std::vector<char> ref_decompression = BrotliIStream::decompress(ref_compression, DATA_REF.size());

    EXPECT_EQ(DATA_REF, ref_decompression);
}

TEST(Brotli, StreamCompression_Decompression)
{
    std::string serialized = write_at_once_to_stream<BrotliOStream>(DATA_REF);
    std::vector<char> test_serialized(serialized.begin(), serialized.end());

    std::vector<char> ref_decompression = BrotliIStream::decompress(test_serialized, DATA_REF.size()*2);

    EXPECT_EQ(DATA_REF, ref_decompression);
}

TEST(Brotli, Compression_StreamDecompression)
{
    std::vector<char> ref_compressed = BrotliOStream::compress(DATA_REF);
    std::string compressed(ref_compressed.begin(), ref_compressed.end());

    std::vector<char> test_decompressed = read_at_once_from_stream<BrotliIStream>(compressed, DATA_REF.size());

    EXPECT_EQ(DATA_REF, test_decompressed);
}

