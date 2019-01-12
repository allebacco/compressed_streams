#ifndef COMPRESSED_STREAMS_ISTREAM_VIEW_H
#define COMPRESSED_STREAMS_ISTREAM_VIEW_H

#include <istream>

namespace compressed_streams
{

class IStreamView: public std::istream
{
public:
    IStreamView(const char* data, const size_t size);

    virtual ~IStreamView();
};

} // namespace compressed_streams

#endif // COMPRESSED_STREAMS_ISTREAM_VIEW_H
