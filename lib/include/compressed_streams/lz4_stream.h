#ifndef COMPRESSED_STREAMS_LZ4_STREAM_H
#define COMPRESSED_STREAMS_LZ4_STREAM_H

#include <iostream>

namespace compressed_streams
{

class Lz4OStream: public std::ostream
{
public:
    static constexpr int DEFAULT_COMPRESSION_LEVEL = 0;
    static constexpr int BEST_SPEED = 1;
    static constexpr int BEST_COMPRESSION = 9;

public:
    Lz4OStream(std::ostream &stream, int compression_level=DEFAULT_COMPRESSION_LEVEL);

    virtual ~Lz4OStream();
};


}   // namespace compressed_streams

#endif // COMPRESSED_STREAMS_LZ4_STREAM_H
