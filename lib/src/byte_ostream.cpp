#include "../include/compressed_streams/byte_ostream.h"

#include <vector>

namespace compressed_streams
{


class ByteOStreamBuf final: public std::streambuf
{
public:
    ByteOStreamBuf()
    {
        // setup the write are buffer. Last byte is for the overflow operation
        setp(&m_buffer.front(), &m_buffer.front() + m_buffer.size());
    }

    virtual ~ByteOStreamBuf()
    {}

    const char* get_buffer() const { return m_buffer.data(); }

    size_t size() const { return std::distance(pbase(), pptr()); }

protected:

    virtual int_type overflow(int_type ch) override
    {
        m_buffer.push_back(ch);
        size_t current_size = m_buffer.size();

        // adjust the size of the buffer to the vector capacity
        m_buffer.reserve(m_buffer.capacity());
        setp(&m_buffer.front(), &m_buffer.front() + m_buffer.size());
        pbump(current_size);

        return ch;
    }

private:

    std::vector<char> m_buffer;
};




ByteOStream::ByteOStream():
    std::ostream(new ByteOStreamBuf())
{

}

ByteOStream::~ByteOStream()
{
    delete rdbuf();
}

const char* ByteOStream::get_bytes() const
{
    ByteOStreamBuf* streambuf = reinterpret_cast<ByteOStreamBuf*>(rdbuf());
    if(streambuf != nullptr)
        return streambuf->get_buffer();

    return nullptr;
}

size_t ByteOStream::size() const
{
    ByteOStreamBuf* streambuf = reinterpret_cast<ByteOStreamBuf*>(rdbuf());
    if(streambuf != nullptr)
        return streambuf->size();

    return 0;
}


} // namespace compressed_streams
