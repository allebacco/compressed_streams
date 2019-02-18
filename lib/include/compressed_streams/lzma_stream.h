#ifndef COMPRESSED_STREAMS_LZMA_STREAM_H
#define COMPRESSED_STREAMS_LZMA_STREAM_H

#include <iostream>
#include <vector>

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

    static std::vector<char> compress(const std::vector<char>& data);
};


class LzmaIStream: public std::istream
{
public:
    static constexpr size_t AUTO_SIZE = 0;

public:
    LzmaIStream(std::istream& stream);

    virtual ~LzmaIStream();

    static std::vector<char> decompress(const std::vector<char>& data, size_t expected_size=AUTO_SIZE);
};



} // namespace compressed_streams


#endif // COMPRESSED_STREAMS_LZMA_STREAM_H
