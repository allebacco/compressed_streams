#ifndef COMPRESSED_STREAMS_LZMA_STREAM_H
#define COMPRESSED_STREAMS_LZMA_STREAM_H

#include <iostream>

namespace compressed_streams
{

class LzmaOStream: public std::ostream
{
public:
    static constexpr int DEFAULT_COMPRESSION_LEVEL = 6;
    static constexpr int BEST_SPEED = 0;
    static constexpr int BEST_COMPRESSION = 9;

public:
    LzmaOStream(std::ostream &stream, int compression_level=DEFAULT_COMPRESSION_LEVEL);

    virtual ~LzmaOStream();
};


class LzmaIStream: public std::istream
{
public:
    LzmaIStream(std::istream& stream);

    virtual ~LzmaIStream();
};



} // namespace compressed_streams


#endif // COMPRESSED_STREAMS_LZMA_STREAM_H
