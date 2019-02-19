
#include "../include/compressed_streams/lzma_stream.h"

#include "common.h"

using namespace compressed_streams;


using LzmaIOStreamTypes = ::testing::Types<std::tuple<LzmaOStream, LzmaIStream>>;
INSTANTIATE_TYPED_TEST_CASE_P(LzmaIOStreamTest, IOStreamTest, LzmaIOStreamTypes);


TEST(Lzma, Compression_Decompression)
{
    std::vector<char> ref_compression = LzmaOStream::compress(DATA_REF);
    std::vector<char> ref_decompression = LzmaIStream::decompress(ref_compression, DATA_REF.size()*2);

    EXPECT_EQ(DATA_REF, ref_decompression);
}

TEST(Lzma, Compression_Decompression_WithTooSmallDecompressedSize)
{
    std::vector<char> ref_compression = LzmaOStream::compress(DATA_REF);
    std::vector<char> ref_decompression = LzmaIStream::decompress(ref_compression, 1);

    EXPECT_EQ(DATA_REF, ref_decompression);
}


TEST(Lzma, Compression_Decompression_WithDefaultDecompressedSize)
{
    std::vector<char> ref_compression = LzmaOStream::compress(DATA_REF);
    std::vector<char> ref_decompression = LzmaIStream::decompress(ref_compression);

    EXPECT_EQ(DATA_REF, ref_decompression);
}


TEST(Lzma, StreamCompression_Decompression)
{
    std::string serialized = write_at_once_to_stream<LzmaOStream>(DATA_REF);
    std::vector<char> test_serialized(serialized.begin(), serialized.end());

    std::vector<char> ref_decompression = LzmaIStream::decompress(test_serialized, DATA_REF.size()*2);

    EXPECT_EQ(DATA_REF, ref_decompression);
}


TEST(Lzma, Compression_StreamDecompression)
{
    std::vector<char> ref_compressed = LzmaOStream::compress(DATA_REF);
    std::string compressed(ref_compressed.begin(), ref_compressed.end());

    std::vector<char> test_decompressed = read_at_once_from_stream<LzmaIStream>(compressed, DATA_REF.size());

    EXPECT_EQ(DATA_REF, test_decompressed);
}
