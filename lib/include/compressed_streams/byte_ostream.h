#ifndef COMPRESSED_STREAMS_BYTE_OSTREAM_H
#define COMPRESSED_STREAMS_BYTE_OSTREAM_H


#include <ostream>

namespace compressed_streams
{

class ByteOStream: public std::ostream
{
public:
    ByteOStream();

    virtual ~ByteOStream();

    const char* get_bytes() const;

    size_t size() const;
};

}   // namespace compressed_streams

#endif // COMPRESSED_STREAMS_BYTE_OSTREAM_H
