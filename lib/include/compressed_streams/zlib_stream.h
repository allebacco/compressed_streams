#ifndef COMPRESSED_STREAMS_ZLIB_STREAM_H
#define COMPRESSED_STREAMS_ZLIB_STREAM_H

#include <iostream>

namespace compressed_streams
{

class ZlibOStream: public std::ostream
{
public:
    static constexpr int DEFAULT_COMPRESSION_LEVEL = -1;
    static constexpr int NO_COMPRESSION = 0;
    static constexpr int BEST_SPEED = 1;
    static constexpr int BEST_COMPRESSION = 9;

public:
    ZlibOStream(std::ostream &stream, int compression_level=DEFAULT_COMPRESSION_LEVEL);

    virtual ~ZlibOStream();
};


class ZlibIStream: public std::istream
{
public:
    ZlibIStream(std::istream &stream);

    virtual ~ZlibIStream();
};



}   // namespace compressed_streams



#endif // COMPRESSED_STREAMS_ZLIB_STREAM_H
