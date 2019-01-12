
#include "../include/compressed_streams/lz4_stream.h"

#include <vector>
#include <cstring>
#include <lz4frame.h>

namespace compressed_streams
{

static constexpr LZ4F_preferences_t DEFAULT_PREFERENCES = {
    {
        LZ4F_max256KB, LZ4F_blockLinked, LZ4F_noContentChecksum, LZ4F_frame,
        0 /* unknown content size */,
        0 /* no dictID */ ,
        LZ4F_noBlockChecksum
    },
    0,   /* compression level; 0 == default */
    0,   /* autoflush */
    0,   /* favor decompression speed */
    { 0, 0, 0 },  /* reserved, must be set to 0 */
};


class Lz4OStreamBuf final: public std::streambuf
{
public:
    Lz4OStreamBuf(std::streambuf* buffer, int compression_level=0):
        m_sink(buffer),
        m_preferences(DEFAULT_PREFERENCES)
    {
        size_t ctx_creation = LZ4F_createCompressionContext(&m_compression_ctx, LZ4F_VERSION);
        if(LZ4F_isError(ctx_creation))
            throw std::runtime_error("Error during LZ4 stream creation");

        m_preferences.compressionLevel = compression_level;

        // setup buffers
        size_t internal_buffer_size = LZ4F_compressBound(0, &m_preferences);
        internal_buffer_size = std::max<size_t>(internal_buffer_size, LZ4F_HEADER_SIZE_MAX) +1;
        m_write_area.resize(internal_buffer_size);
        m_compression_buffer.resize(internal_buffer_size);

        // setup the write are buffer. Last byte is for the overflow operation
        setp(&m_write_area.front(), &m_write_area.front() + m_write_area.size() -1);

        initialize_stream();
    }

    virtual ~Lz4OStreamBuf()
    {
        finalize_stream();
        LZ4F_freeCompressionContext(m_compression_ctx);
    }

protected:

    virtual int_type overflow(int_type ch) override
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

    void initialize_stream()
    {
        size_t header_size = LZ4F_compressBegin(
                    m_compression_ctx, m_compression_buffer.data(), m_compression_buffer.size(), &m_preferences
        );
        if(LZ4F_isError(header_size))
            throw std::runtime_error("Error during LZ4 stream initialization");

        m_sink->sputn(m_compression_buffer.data(), header_size);
    }

    void finalize_stream()
    {
        compress_buffer();

        size_t compressed_size = LZ4F_compressEnd(
            m_compression_ctx,
            m_compression_buffer.data(), m_compression_buffer.size(),
            nullptr
        );

        if(LZ4F_isError(compressed_size))
            throw std::runtime_error("Error during LZ4 stream finalization");

        m_sink->sputn(m_compression_buffer.data(), compressed_size);
    }

    size_t compress_buffer()
    {
        size_t num_bytes = std::distance(pbase(), pptr());

        size_t compressed_size = LZ4F_compressUpdate(
            m_compression_ctx,
            m_compression_buffer.data(), m_compression_buffer.size(),
            pbase(), num_bytes,
            nullptr
        );

        if(LZ4F_isError(compressed_size))
            throw std::runtime_error("Error during LZ4 stream writing");

        setp(&m_write_area.front(), &m_write_area.front() + m_write_area.size() -1);

        return m_sink->sputn(m_compression_buffer.data(), compressed_size);
    }

private:
    std::streambuf* m_sink;
    LZ4F_cctx* m_compression_ctx;
    LZ4F_preferences_t m_preferences;
    std::vector<char> m_write_area;
    std::vector<char> m_compression_buffer;
};


class Lz4IStreamBuf final: public std::streambuf
{
public:
    Lz4IStreamBuf(std::streambuf* source, size_t internal_buffer_size=4096):
        m_source(source),
        m_context(nullptr),
        m_src_buffer(internal_buffer_size),
        m_read_area(internal_buffer_size),
        m_src_offset(0),
        m_src_size(0)
    {
        size_t status = LZ4F_createDecompressionContext(&m_context, LZ4F_VERSION);
        if(LZ4F_isError(status))
            throw std::runtime_error("Error during LZ4 istream creation");

        setg(&m_read_area.front(), &m_read_area.front(), &m_read_area.front());
    }

    virtual ~Lz4IStreamBuf()
    {
        LZ4F_freeDecompressionContext(m_context);
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

            size_t src_avalable = m_src_size - m_src_offset;
            size_t dest_size = m_read_area.size();
            size_t ret = LZ4F_decompress(
                m_context,
                &m_read_area.front(), &dest_size,
                &m_src_buffer.front() + m_src_offset, &src_avalable, nullptr
            );
            m_src_offset += src_avalable;

            if (LZ4F_isError(ret) != 0)
                throw std::runtime_error("Error during LZ4 decompression");

            if(dest_size > 0)
            {
                setg(&m_read_area.front(), &m_read_area.front(), &m_read_area.front() + dest_size);
                return traits_type::to_int_type(*gptr());
            }
        }

        return traits_type::eof();
    }

private:
    std::streambuf* m_source;
    LZ4F_dctx* m_context;
    std::vector<char> m_src_buffer;
    std::vector<char> m_read_area;
    size_t m_src_offset;
    size_t m_src_size;
};


Lz4OStream::Lz4OStream(std::ostream &stream, int compression_level):
    std::ostream(new Lz4OStreamBuf(stream.rdbuf(), compression_level))
{}

Lz4OStream::~Lz4OStream()
{
    delete rdbuf();
}

Lz4IStream::Lz4IStream(std::istream& stream):
    std::istream(new Lz4IStreamBuf(stream.rdbuf()))
{}


Lz4IStream::~Lz4IStream()
{
    delete rdbuf();
}

}   // namespace compressed_streams
