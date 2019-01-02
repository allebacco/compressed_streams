#ifndef COMPRESSED_STREAMS_ISTREAMBUF_VIEW_H
#define COMPRESSED_STREAMS_ISTREAMBUF_VIEW_H

#include <iostream>

namespace compressed_streams
{

class IStreamBufView: public std::streambuf
{
public:
    static_assert(1 == sizeof(char), "sizeof buffer element type 1.");

    IStreamBufView(const char* data, size_t len):
        m_begin(data), m_end(data + len), m_current(data)
    {}

protected:

    int_type underflow() override
    {
        return (m_current == m_end ? traits_type::eof() : traits_type::to_int_type(*m_current));
    }

    int_type uflow() override
    {
        return (m_current == m_end ? traits_type::eof() : traits_type::to_int_type(*m_current++));
    }

    std::streamsize showmanyc() override
    {
        return m_end - m_current;
    }

private:

    const char* const m_begin;
    const char* const m_end;
    const char* m_current;
};


class IStreamView: public std::istream
{
public:
    IStreamView(const char* data, const size_t size):
        std::istream(new IStreamBufView(data, size))
    {}

    virtual ~IStreamView()
    {
        delete rdbuf();
    }
};

} // namespace compressed_streams

#endif // COMPRESSED_STREAMS_ISTREAMBUF_VIEW_H
