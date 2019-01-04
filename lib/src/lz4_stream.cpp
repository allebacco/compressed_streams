
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


class Lz4OStreamBuf: public std::streambuf
{
public:
    Lz4OStreamBuf(std::streambuf* buffer, int compression_level=0):
        m_buffer(buffer),
        m_preferences(DEFAULT_PREFERENCES)
    {
        size_t ctx_creation = LZ4F_createCompressionContext(&m_compression_ctx, LZ4F_VERSION);
        if(LZ4F_isError(ctx_creation))
            throw std::runtime_error("Error during LZ4 stream creation");

        m_preferences.compressionLevel = compression_level;

        size_t internal_buffer_size = LZ4F_compressBound(0, &m_preferences);
        m_tmp_buffer.resize(internal_buffer_size);

        initialize_stream();
    }

    virtual ~Lz4OStreamBuf()
    {
        finalize_stream();
        LZ4F_freeCompressionContext(m_compression_ctx);
    }

protected:

    virtual std::streamsize xsputn(const char_type* data, std::streamsize data_size) override
    {
        size_t compressed_size = LZ4F_compressUpdate(
            m_compression_ctx,
            m_tmp_buffer.data(), m_tmp_buffer.size(),
            data, data_size,
            nullptr
        );

        if(LZ4F_isError(compressed_size))
            throw std::runtime_error("Error during LZ4 stream writing");

        return m_buffer->sputn(m_tmp_buffer.data(), compressed_size);
    }

    virtual int_type overflow(int_type ch) override
    {
        const char data = traits_type::to_char_type(ch);
        return static_cast<int_type>(xsputn(&data, 1));
    }

private:

    void initialize_stream()
    {
        size_t header_size = LZ4F_compressBegin(m_compression_ctx, m_tmp_buffer.data(), m_tmp_buffer.size(), &m_preferences);
        if(LZ4F_isError(header_size))
            throw std::runtime_error("Error during LZ4 stream initialization");

        m_buffer->sputn(m_tmp_buffer.data(), header_size);
    }

    void finalize_stream()
    {
        size_t compressed_size = LZ4F_compressEnd(
            m_compression_ctx,
            m_tmp_buffer.data(), m_tmp_buffer.size(),
            nullptr
        );

        if(LZ4F_isError(compressed_size))
            throw std::runtime_error("Error during LZ4 stream finalization");

        m_buffer->sputn(m_tmp_buffer.data(), compressed_size);
    }

private:
    std::streambuf* m_buffer;
    LZ4F_cctx* m_compression_ctx;
    LZ4F_preferences_t m_preferences;
    std::vector<char> m_tmp_buffer;
};



Lz4OStream::Lz4OStream(std::ostream &stream, int compression_level):
    std::ostream(new Lz4OStreamBuf(stream.rdbuf(), compression_level))
{}

Lz4OStream::~Lz4OStream()
{
    delete rdbuf();
}



}   // namespace compressed_streams
