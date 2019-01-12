
#include "../include/compressed_streams/zstd_stream.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <zstd.h>

namespace compressed_streams
{

class ZstdOStreamBuf final: public std::streambuf
{
public:
    ZstdOStreamBuf(std::streambuf* buffer, int compression_level=ZSTD_CLEVEL_DEFAULT):
        m_sink(buffer),
        m_context(ZSTD_createCStream()),
        m_input_buffer(ZSTD_CStreamInSize()),
        m_compression_buffer(ZSTD_CStreamOutSize())
    {
        // setup the write are buffer. Last byte is for the overflow operation
        setp(&m_input_buffer.front(), &m_input_buffer.front() + m_input_buffer.size() -1);

        initialize_stream(compression_level);
    }

    virtual ~ZstdOStreamBuf()
    {
        finalize_stream();

        ZSTD_freeCStream(m_context);
    }

protected:

    int_type overflow(int_type ch) override
    {
        *pptr() = traits_type::to_char_type(ch);
        pbump(1);

        compress_buffer();

        return ch;
    }

    virtual int sync() override
    {
        compress_buffer();
        return m_sink->pubsync();
    }

private:

    void initialize_stream(int compression_level)
    {
        ZSTD_initCStream(m_context, compression_level);
    }

    void finalize_stream()
    {
        compress_buffer();

        size_t ret = 0;
        do
        {
            ZSTD_outBuffer output = { m_compression_buffer.data(), m_compression_buffer.size(), 0 };

            ret = ZSTD_endStream(m_context, &output);
            if (!ZSTD_isError(ret))
                m_sink->sputn(m_compression_buffer.data(), output.pos);

        } while(ret > 0 && !ZSTD_isError(ret));
    }

    void compress_buffer()
    {
        size_t num_bytes = std::distance(pbase(), pptr());
        ZSTD_inBuffer input {pbase(), num_bytes, 0};

        while(input.pos < input.size)
        {
            ZSTD_outBuffer output = {m_compression_buffer.data(), m_compression_buffer.size(), 0};
            size_t ret = ZSTD_compressStream(m_context, &output , &input);
            if(ZSTD_isError(ret))
                throw std::runtime_error("Error during ZSTD stream write");

            m_sink->sputn(m_compression_buffer.data(), output.pos);
        }

        setp(&m_input_buffer.front(), &m_input_buffer.front() + m_input_buffer.size() -1);
    }

private:
    std::streambuf* m_sink;
    ZSTD_CStream* m_context;
    std::vector<char> m_input_buffer;
    std::vector<char> m_compression_buffer;
};


class ZstdIStreamBuf final: public std::streambuf
{
public:

    ZstdIStreamBuf(std::streambuf* source):
        m_source(source),
        m_context(ZSTD_createDStream()),
        m_src_buffer(ZSTD_CStreamInSize()),
        m_read_area(ZSTD_CStreamOutSize()),
        m_src_offset(0),
        m_src_size(0)
    {
        ZSTD_initDStream(m_context);

        setg(&m_read_area.front(), &m_read_area.front(), &m_read_area.front());
    }

    virtual ~ZstdIStreamBuf()
    {
        ZSTD_freeDStream(m_context);
    }

protected:

    virtual int_type underflow() override
    {
        while(true)
        {
            if(m_src_offset == m_src_size)
            {
                m_src_size = m_source->sgetn(&m_src_buffer.front(), m_src_buffer.size());
                m_src_offset = 0;
            }

            if(m_src_size == 0)
                return traits_type::eof();

            ZSTD_inBuffer input = {&m_src_buffer.front(), m_src_size, m_src_offset};
            ZSTD_outBuffer output = {&m_read_area.front(), m_read_area.size(), 0};
            size_t ret = ZSTD_decompressStream(m_context, &output, &input);
            m_src_offset = input.pos;

            if (ZSTD_isError(ret) != 0)
                throw std::runtime_error("Error during ZSTD decompression");

            if(output.pos > 0)
            {
                setg(&m_read_area.front(), &m_read_area.front(), &m_read_area.front() + output.pos);
                return traits_type::to_int_type(*gptr());
            }
        }

        return traits_type::eof();
    }

private:
    std::streambuf* m_source;
    ZSTD_DStream* m_context;
    std::vector<char> m_src_buffer;
    std::vector<char> m_read_area;
    size_t m_src_offset;
    size_t m_src_size;
};


ZstdOStream::ZstdOStream(std::ostream& stream, int compression_level):
    std::ostream(new ZstdOStreamBuf(stream.rdbuf(), compression_level))
{}

ZstdOStream::~ZstdOStream()
{
    delete rdbuf();
}

ZstdIStream::ZstdIStream(std::istream& stream):
    std::istream(new ZstdIStreamBuf(stream.rdbuf()))
{}

ZstdIStream::~ZstdIStream()
{
    delete rdbuf();
}



}   // namespace compressed_streams
