
#include "../include/compressed_streams/lzma_stream.h"

#include "common.h"

using namespace compressed_streams;


//using LzmaIOStreamTypes = ::testing::Types<std::tuple<LzmaOStream, LzmaIStream>>;
//INSTANTIATE_TYPED_TEST_SUITE_P(LzmaIOStreamTest, IOStreamTest, LzmaIOStreamTypes);


TEST(Lzma, Compression_Decompression)
{
    std::vector<char> ref_compression = LzmaOStream::compress(DATA_REF);
    std::vector<char> ref_decompression = LzmaIStream::decompress(ref_compression, DATA_REF.size()*2);

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


TEST(LzmaStream, WriteAllAtOnce)
{
    std::string serialized = write_at_once_to_stream<LzmaOStream>(DATA_REF);

    std::vector<char> test_serialized(serialized.begin(), serialized.end());

    std::cout << "Serialized size = " << serialized.size() <<std::endl;

    std::vector<char> ref_compression = LzmaOStream::compress(DATA_REF);

    std::cout << "ref_compression size = " << ref_compression.size() <<std::endl;

    std::vector<char> ref_decompression = LzmaIStream::decompress(ref_compression, DATA_REF.size()*2);

    EXPECT_EQ(DATA_REF, ref_decompression);

    /*
    std::vector<char> test_data = read_at_once_from_stream<LzmaIStream>(serialized, DATA_REF.size());

    EXPECT_EQ(DATA_REF.size(), test_data.size());

    //EXPECT_EQ(DATA_REF, test_data) << "Serialized size: " << serialized.size();

    for(int i=0; i<DATA_REF.size(); ++i)
        ASSERT_EQ(DATA_REF[i], test_data[i]) << " at " << i;
        */
}

