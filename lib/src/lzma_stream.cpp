
#include "../include/compressed_streams/lzma_stream.h"

#include <lzma.h>

#include <vector>
#include <cstring>
#include <algorithm>

namespace compressed_streams
{


class LzmaOStreamBuf final: public std::streambuf
{
public:
    LzmaOStreamBuf(std::streambuf* buffer, int compression_level=0, size_t buffer_size=BUFSIZ):
        m_sink(buffer),
        m_lzma_stream(LZMA_STREAM_INIT),
        m_write_area(buffer_size),
        m_compression_buffer(buffer_size)
    {
        // setup the write are buffer. Last byte is for the overflow operation
        setp(&m_write_area.front(), &m_write_area.front() + m_write_area.size() -1);

        initialize_stream(compression_level);
    }

    virtual ~LzmaOStreamBuf()
    {
        lzma_end(&m_lzma_stream);
        finalize_stream();
    }

protected:

    virtual int_type overflow(int_type ch) override
    {
        *pptr() = traits_type::to_char_type(ch);
        pbump(1);

        compress_buffer(LZMA_RUN);

        return ch;
    }

    virtual int sync() override
    {
        compress_buffer(LZMA_RUN);
        return m_sink->pubsync();
    }

private:

    void initialize_stream(int compression_level)
    {
        // Initialize the encoder using a preset. Set the integrity to check
        // to CRC64, which is the default in the xz command line tool. If
        // the .xz file needs to be decompressed with XZ Embedded, use
        // LZMA_CHECK_CRC32 instead.
        lzma_ret ret = lzma_easy_encoder(&m_lzma_stream, compression_level, LZMA_CHECK_CRC64 );

        // Return successfully if the initialization went fine.
        if(ret != LZMA_OK)
        {
            switch( ret )
            {
                case LZMA_MEM_ERROR:
                    throw std::runtime_error("Memory allocation failed");
                case LZMA_OPTIONS_ERROR:
                    throw std::invalid_argument("Specified compression level is not supported");
                case LZMA_UNSUPPORTED_CHECK:
                    throw std::invalid_argument("Specified integrity check is not supported");
                default:
                    throw std::runtime_error("Unknown error: " + std::to_string(ret));
            }
        }
    }

    void finalize_stream()
    {
        compress_buffer(LZMA_FINISH);
    }

    size_t compress_buffer(lzma_action action)
    {
        size_t num_bytes = std::distance(pbase(), pptr());

        m_lzma_stream.next_in = reinterpret_cast<const uint8_t*>(pbase());
        m_lzma_stream.avail_in = num_bytes;
        m_lzma_stream.total_in = 0;
        m_lzma_stream.next_out = reinterpret_cast<uint8_t*>(m_compression_buffer.data());
        m_lzma_stream.avail_out = m_compression_buffer.size();
        m_lzma_stream.total_out = 0;

        size_t sputn_return = 0;
        while(m_lzma_stream.avail_in > 0)
        {
            lzma_ret ret = lzma_code(&m_lzma_stream, action);

            if(ret != LZMA_OK && ret != LZMA_STREAM_END)
            {
                switch(ret)
                {
                case LZMA_MEM_ERROR:
                    throw std::runtime_error("Memory allocation failed");
                case LZMA_DATA_ERROR:
                    throw std::out_of_range("File size limits exceeded");
                default:
                    throw std::runtime_error("Unknown error: " + std::to_string(ret));
                }
            }

            if(m_lzma_stream.total_out > 0)
            {
                sputn_return += m_sink->sputn(m_compression_buffer.data(), m_lzma_stream.total_out);
                m_lzma_stream.next_out = reinterpret_cast<uint8_t*>(m_compression_buffer.data());
                m_lzma_stream.avail_out = m_compression_buffer.size();
                m_lzma_stream.total_out = 0;
            }
        }

        setp(&m_write_area.front(), &m_write_area.front() + m_write_area.size() -1);

        return sputn_return;
    }

private:
    std::streambuf* m_sink;
    lzma_stream m_lzma_stream;

    std::vector<char> m_write_area;
    std::vector<char> m_compression_buffer;
};














LzmaOStream::LzmaOStream(std::ostream& stream, int compression_level):
    std::ostream(new LzmaOStreamBuf(stream.rdbuf(), compression_level))
{

}

LzmaOStream::~LzmaOStream()
{

}

LzmaIStream::LzmaIStream(std::istream& stream)
{

}

LzmaIStream::~LzmaIStream()
{

}

}   // namespace compressed_streams
