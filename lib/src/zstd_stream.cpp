
#include "../include/compressed_streams/zstd_stream.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <zstd.h>

namespace compressed_streams
{

class ZstdOStreamBuf: public std::streambuf
{
public:
    ZstdOStreamBuf(std::streambuf* buffer, int compression_level=ZSTD_CLEVEL_DEFAULT, size_t internal_buffer_size=4096):
        m_buffer(buffer),
        m_zstd_cstream(ZSTD_createCStream()),
        m_tmp_buffer(internal_buffer_size)
    {
        ZSTD_initCStream(m_zstd_cstream, compression_level);
    }

    virtual ~ZstdOStreamBuf()
    {
        size_t ret = 0;
        do
        {
            ZSTD_outBuffer output = { m_tmp_buffer.data(), m_tmp_buffer.size(), 0 };

            ret = ZSTD_endStream(m_zstd_cstream, &output);
            if (!ZSTD_isError(ret))
                m_buffer->sputn(m_tmp_buffer.data(), output.pos);

        } while(ret > 0 && !ZSTD_isError(ret));

        ZSTD_freeCStream(m_zstd_cstream);
    }

protected:

    std::streamsize xsputn(const char_type* data, std::streamsize data_size) override
    {
        std::streamsize bytes_written = 0;
        ZSTD_inBuffer input {data, static_cast<size_t>(data_size), 0};

        while (input.pos < input.size)
        {
            ZSTD_outBuffer output = { m_tmp_buffer.data(), m_tmp_buffer.size(), 0 };
            size_t toRead = ZSTD_compressStream(m_zstd_cstream, &output , &input);
            if (ZSTD_isError(toRead)) {

            }
            bytes_written += m_buffer->sputn(m_tmp_buffer.data(), output.pos);
        }

        return bytes_written;
    }

    int_type overflow(int_type ch) override
    {
        const char data = traits_type::to_char_type(ch);
        return static_cast<int_type>(xsputn(&data, 1));
    }

    virtual int sync() override
    {
        size_t ret = 0;
        do
        {
            ZSTD_outBuffer output = { m_tmp_buffer.data(), m_tmp_buffer.size(), 0 };

            ret = ZSTD_flushStream(m_zstd_cstream, &output);
            if (ZSTD_isError(ret))
                throw std::logic_error("Compression finalization error");

            m_buffer->sputn(m_tmp_buffer.data(), output.pos);

        } while(ret > 0);

        return m_buffer->pubsync();
    }

private:
    std::streambuf* m_buffer;
    ZSTD_CStream* m_zstd_cstream;
    std::vector<char> m_tmp_buffer;
};


class ZstdIStreamBuf: public std::streambuf
{
public:
    ZstdIStreamBuf(std::streambuf* buffer, size_t internal_buffer_size=4096):
        m_src_buffer(buffer),
        m_zstd_dstream(ZSTD_createDStream()),
        m_tmp_buffer_memory(internal_buffer_size)
    {
        m_input_buffer = {m_tmp_buffer_memory.data(), 0, 0};
        ZSTD_initDStream(m_zstd_dstream);
    }

    virtual ~ZstdIStreamBuf()
    {
        ZSTD_freeDStream(m_zstd_dstream);
    }

protected:

    virtual int_type underflow() override
    {
        char value;
        std::streamsize read_count = sgetn(&value, 1);
        return read_count == 0 ? traits_type::eof() : traits_type::to_int_type(value);
    }

    size_t fill_internal_buffer()
    {
        // if all data has been consumed, clean the buffer position
        if(m_input_buffer.pos == m_input_buffer.size)
            m_input_buffer.pos = 0;

        // compute buffer destination
        const char* buffer_ptr = reinterpret_cast<const char*>(m_input_buffer.src) + m_input_buffer.pos;
        const std::streamsize max_read_count = m_tmp_buffer_memory.size() - m_input_buffer.pos;

        std::streamsize read_count = m_src_buffer->sgetn(const_cast<char*>(buffer_ptr), max_read_count);
        if(read_count <= 0)
            return 0;

        m_input_buffer.size = m_input_buffer.pos + read_count;

        return read_count;
    }

    virtual std::streamsize xsgetn(char_type* dest, std::streamsize dest_size) override
    {
        // Load already decompressed data
        size_t available = m_input_buffer.size - m_input_buffer.pos;
        size_t copy_count = std::min<size_t>(dest_size, available);
        if(copy_count > 0)
        {
            std::memcpy(dest, reinterpret_cast<const char*>(m_input_buffer.src) + m_input_buffer.pos, copy_count);
            m_input_buffer.pos += copy_count;
        }

        if(copy_count == dest_size)
            return copy_count;

        // Decompress data
        ZSTD_outBuffer output = { dest, static_cast<size_t>(dest_size), copy_count };
        do
        {
            size_t read_count = fill_internal_buffer();
            if(read_count == 0)
                return static_cast<std::streamsize>(output.pos);

            size_t ret = ZSTD_decompressStream(m_zstd_dstream, &output, &m_input_buffer);
            if(ZSTD_isError(ret))
                throw std::logic_error("Decompression error");
        } while(output.pos < output.size);

        return static_cast<std::streamsize>(output.pos);
    }

private:
    std::streambuf* m_src_buffer;
    ZSTD_DStream* m_zstd_dstream;
    std::vector<char> m_tmp_buffer_memory;
    ZSTD_inBuffer m_input_buffer;
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
