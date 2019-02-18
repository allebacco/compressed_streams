
#include "common.h"

#include "../include/compressed_streams/brotli_stream.h"

using namespace compressed_streams;


TEST(Brotli, Compression_Decompression)
{
    std::vector<char> ref_compression = BrotliOStream::compress(DATA_REF);
    std::vector<char> ref_decompression = BrotliIStream::decompress(ref_compression, DATA_REF.size());

    EXPECT_EQ(DATA_REF, ref_decompression);
}

