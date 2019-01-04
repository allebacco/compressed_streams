
#include "../include/compressed_streams/zlib_stream.h"

#include "internal/zstr.hpp"

namespace compressed_streams
{

static constexpr size_t DEFAULT_BUFFER_SIZE = 1 << 20;


ZlibOStream::ZlibOStream(std::ostream& stream, int compression_level):
    std::ostream(new zstr::ostreambuf(stream.rdbuf(), DEFAULT_BUFFER_SIZE, compression_level, false))
{}

ZlibOStream::~ZlibOStream()
{
    delete rdbuf();
}

ZlibIStream::ZlibIStream(std::istream& stream):
    std::istream(new zstr::istreambuf(stream.rdbuf(), DEFAULT_BUFFER_SIZE))
{}

ZlibIStream::~ZlibIStream()
{
    delete rdbuf();
}




}   // namespace compressed_streams
