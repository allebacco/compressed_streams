
#include "../include/compressed_streams/lzma_stream.h"

#include <lzma.h>

#include <vector>
#include <cstring>
#include <algorithm>
#include <cassert>

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
        initialize_stream(compression_level);

        // setup the write are buffer. Last byte is for the overflow operation
        setp(&m_write_area.front(), &m_write_area.front() + m_write_area.size() -1);

        m_lzma_stream.next_in = nullptr;
        m_lzma_stream.avail_in = 0;
        m_lzma_stream.next_out = reinterpret_cast<uint8_t*>(m_compression_buffer.data());
        m_lzma_stream.avail_out = m_compression_buffer.size();
    }

    virtual ~LzmaOStreamBuf()
    {
        finalize_stream();
        lzma_end(&m_lzma_stream);
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

    void initialize_stream(int compression_level)
    {
        // Initialize the encoder using a preset. Set the integrity to check
        // to CRC64, which is the default in the xz command line tool.
        lzma_ret ret = lzma_easy_encoder(&m_lzma_stream, compression_level, LZMA_CHECK_CRC64);
        throw_if_error(ret);
    }

    void finalize_stream()
    {
        compress_buffer();

        // Read all the compressed data from the stream

        m_lzma_stream.next_in = nullptr;
        m_lzma_stream.avail_in = 0;

        while(true)
        {
            lzma_ret ret = lzma_code(&m_lzma_stream, LZMA_FINISH);
            throw_if_error(ret);

            if(m_lzma_stream.total_out > 0)
            {
                m_sink->sputn(m_compression_buffer.data(), m_lzma_stream.total_out);

                m_lzma_stream.next_out = reinterpret_cast<uint8_t*>(m_compression_buffer.data());
                m_lzma_stream.avail_out = m_compression_buffer.size();
                m_lzma_stream.total_out = 0;
            }
            else if(ret == LZMA_STREAM_END)
                break;
        }
    }

    void compress_buffer()
    {
        m_lzma_stream.next_in = reinterpret_cast<const uint8_t*>(pbase());
        m_lzma_stream.avail_in = std::distance(pbase(), pptr());

        // Ensure all the input data is written into the lzma stream encoder
        while(m_lzma_stream.avail_in > 0)
        {
            lzma_ret ret = lzma_code(&m_lzma_stream, LZMA_RUN);
            throw_if_error(ret);

            if(m_lzma_stream.total_out > 0)
            {
                m_sink->sputn(m_compression_buffer.data(), m_lzma_stream.total_out);

                m_lzma_stream.next_out = reinterpret_cast<uint8_t*>(m_compression_buffer.data());
                m_lzma_stream.avail_out = m_compression_buffer.size();
                m_lzma_stream.total_out = 0;
            }
        }

        setp(&m_write_area.front(), &m_write_area.front() + m_write_area.size() -1);
    }

    void throw_if_error(lzma_ret ret)
    {
        if(ret == LZMA_OK || ret == LZMA_STREAM_END)
            return;

        switch(ret)
        {
        case LZMA_MEM_ERROR:
            throw std::runtime_error("Memory allocation failed");
        case LZMA_OPTIONS_ERROR:
            throw std::invalid_argument("Specified compression level is not supported");
        case LZMA_UNSUPPORTED_CHECK:
            throw std::invalid_argument("Specified integrity check is not supported");
        case LZMA_DATA_ERROR:
            throw std::out_of_range("File size limits exceeded");
        default:
            throw std::runtime_error("Unknown error: " + std::to_string(ret));
        }
    }

private:
    std::streambuf* m_sink;
    lzma_stream m_lzma_stream;

    std::vector<char> m_write_area;
    std::vector<char> m_compression_buffer;
};





class LzmaIStreamBuf final: public std::streambuf
{
    static constexpr const uint64_t MEMORY_LIMIT_DEFAULT = std::numeric_limits<uint64_t>::max();//100 * 1024*1024; // 100 MB

public:

    LzmaIStreamBuf(std::streambuf* source, size_t buffer_size=BUFSIZ, uint64_t memory_limit=MEMORY_LIMIT_DEFAULT):
        m_source(source),
        m_lzma_stream(LZMA_STREAM_INIT),
        m_src_buffer(buffer_size),
        m_read_area(buffer_size)
    {
        lzma_ret ret = lzma_stream_decoder(&m_lzma_stream, memory_limit, 0);
        throw_if_error(ret);

        setg(&m_read_area.front(), &m_read_area.front(), &m_read_area.front());

        m_lzma_stream.next_in = reinterpret_cast<const uint8_t*>(m_src_buffer.data());
        m_lzma_stream.avail_in = 0;
        m_lzma_stream.next_out = reinterpret_cast<uint8_t*>(m_read_area.data());
        m_lzma_stream.avail_out = m_read_area.size();
    }

    virtual ~LzmaIStreamBuf()
    {
        lzma_end(&m_lzma_stream);
    }

protected:

    virtual int_type underflow() override
    {
        while(true)
        {
            if(m_lzma_stream.avail_in == 0)
            {
                std::streamsize count = m_source->sgetn(m_src_buffer.data(), m_src_buffer.size());
                if(count > 0)
                {
                    m_lzma_stream.avail_in = count;
                    m_lzma_stream.next_in = reinterpret_cast<const uint8_t*>(m_src_buffer.data());
                }
            }

            lzma_ret ret = lzma_code(&m_lzma_stream, LZMA_RUN);
            throw_if_error(ret);

            if(m_lzma_stream.total_out > 0)
            {
                setg(m_read_area.data(), m_read_area.data(), m_read_area.data() + m_lzma_stream.total_out);

                m_lzma_stream.next_out = reinterpret_cast<uint8_t*>(m_read_area.data());
                m_lzma_stream.avail_out = m_read_area.size();
                m_lzma_stream.total_out = 0;

                return traits_type::to_int_type(*gptr());
            }

            if(ret == LZMA_STREAM_END)
                return traits_type::eof();
        }

        return traits_type::eof();
    }

    /*
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

            m_lzma_stream.next_in = reinterpret_cast<const uint8_t*>(&m_src_buffer[m_src_offset]);
            m_lzma_stream.avail_in = m_src_size - m_src_offset;
            m_lzma_stream.next_out = reinterpret_cast<uint8_t*>(&m_read_area[0]);
            m_lzma_stream.avail_out = m_read_area.size();

            lzma_ret ret = lzma_code(&m_lzma_stream, LZMA_RUN);
            throw_if_error(ret);

            m_src_offset = m_lzma_stream.total_in;

            if(m_lzma_stream.total_out > 0)
            {
                setg(&m_read_area.front(), &m_read_area.front(), &m_read_area.front() + m_lzma_stream.total_out);
                return traits_type::to_int_type(*gptr());
            }
        }

        return traits_type::eof();
    }
    */

    void throw_if_error(lzma_ret ret)
    {
        if(ret == LZMA_OK || ret == LZMA_STREAM_END)
            return;

        switch(ret)
        {
        case LZMA_MEM_ERROR:
            throw std::runtime_error("Memory allocation failed");
        case LZMA_FORMAT_ERROR:
            throw std::invalid_argument("Input is not in lzma  format");
        case LZMA_OPTIONS_ERROR:
            throw std::invalid_argument("Unsupported compression options");
        case LZMA_UNSUPPORTED_CHECK:
            throw std::invalid_argument("Specified integrity check is not supported");
        case LZMA_DATA_ERROR:
            throw std::out_of_range("Compressed data is corrupt");
        default:
            throw std::runtime_error("Unknown error: " + std::to_string(ret));
        }
    }

private:
    std::streambuf* m_source;
    lzma_stream m_lzma_stream;
    std::vector<char> m_src_buffer;
    std::vector<char> m_read_area;
};








LzmaOStream::LzmaOStream(std::ostream& stream, int compression_level):
    std::ostream(new LzmaOStreamBuf(stream.rdbuf(), compression_level))
{}

LzmaOStream::~LzmaOStream()
{
    delete rdbuf();
}

std::vector<char> LzmaOStream::compress(const std::vector<char>& data)
{
    std::vector<char> compressed_data(data.size()*2);

    size_t compressed_size = 0;
    lzma_ret ret = lzma_easy_buffer_encode(
        DEFAULT_COMPRESSION_LEVEL, LZMA_CHECK_CRC64, nullptr,
        reinterpret_cast<const uint8_t*>(data.data()), data.size(),
        reinterpret_cast<uint8_t*>(compressed_data.data()), &compressed_size, compressed_data.size()
    );

    assert(ret == LZMA_OK);

    compressed_data.resize(compressed_size);

    return compressed_data;
}

LzmaIStream::LzmaIStream(std::istream& stream):
    std::istream(new LzmaIStreamBuf(stream.rdbuf()))
{}

LzmaIStream::~LzmaIStream()
{
    delete rdbuf();
}

std::vector<char> LzmaIStream::decompress(const std::vector<char>& data, size_t expected_size)
{
    if(expected_size == AUTO_SIZE)
        expected_size = data.size() * 2;

    std::vector<char> decompressed_data(expected_size);

    uint64_t mem_limit = std::numeric_limits<int>::max();
    size_t decompressed_size = 0;
    size_t in_pos = 0;

    lzma_ret ret = LZMA_OK;
    while(true)
    {
        ret = lzma_stream_buffer_decode(
            &mem_limit, 0, nullptr,
            reinterpret_cast<const uint8_t*>(data.data()), &in_pos, data.size(),
            reinterpret_cast<uint8_t*>(decompressed_data.data()), &decompressed_size, decompressed_data.size()
        );

        if(ret == LZMA_BUF_ERROR)
            decompressed_data.resize(decompressed_data.size() * 2);
        else
            break;
    }

    if(ret != LZMA_OK)
        throw std::runtime_error("Error during decompressing");

    decompressed_data.resize(decompressed_size);

    return decompressed_data;
}

}   // namespace compressed_streams
