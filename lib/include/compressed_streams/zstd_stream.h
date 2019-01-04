#ifndef COMPRESSED_STREAMS_ZSTD_STREAM_H
#define COMPRESSED_STREAMS_ZSTD_STREAM_H

#include <iostream>

namespace compressed_streams
{


class ZstdOStream: public std::ostream
{
public:
    static constexpr int DEFAULT_COMPRESSION_LEVEL = 0;
    static constexpr int BEST_SPEED = 1;
    static constexpr int BEST_COMPRESSION = 9;

public:
    ZstdOStream(std::ostream& stream, int compression_level=DEFAULT_COMPRESSION_LEVEL);

    virtual ~ZstdOStream();
};


class ZstdIStream: public std::istream
{
public:
    ZstdIStream(std::istream& stream);

    virtual ~ZstdIStream();
};


}



#endif // COMPRESSED_STREAMS_ZSTD_STREAM_H
