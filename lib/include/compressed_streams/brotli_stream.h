#ifndef COMPRESSED_STREAMS_BROTLI_STREAM_H
#define COMPRESSED_STREAMS_BROTLI_STREAM_H

#include <iostream>
#include <vector>

namespace compressed_streams
{

class BrotliOStream: public std::ostream
{
public:
    static constexpr int DEFAULT_COMPRESSION_LEVEL = 11;
    static constexpr int BEST_SPEED = 0;
    static constexpr int BEST_COMPRESSION = 11;

public:
    BrotliOStream(std::ostream &stream, int compression_level=DEFAULT_COMPRESSION_LEVEL);

    virtual ~BrotliOStream();

public:

    static std::vector<char> compress(const std::vector<char>& data, int compression_level=DEFAULT_COMPRESSION_LEVEL);
};


class BrotliIStream: public std::istream
{
public:
    static constexpr size_t AUTO_SIZE = 0;

public:
    BrotliIStream(std::istream& stream);

    virtual ~BrotliIStream();

public:

    static std::vector<char> decompress(const std::vector<char>& data, size_t expected_size=AUTO_SIZE);
};



} // namespace compressed_streams

#endif // COMPRESSED_STREAMS_BROTLI_STREAM_H
