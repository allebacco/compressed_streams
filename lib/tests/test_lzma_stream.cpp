
#include "../include/compressed_streams/lzma_stream.h"

#include "common.h"

using namespace compressed_streams;


TEST(LzmaStream, WriteAllAtOnce)
{
    write_at_once_to_stream<LzmaOStream>(DATA_REF);
}
