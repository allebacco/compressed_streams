
#include "../include/compressed_streams/brotli_stream.h"

#include <brotli/encode.h>
#include <brotli/decode.h>


namespace compressed_streams
{


class BrotliOStreamBuf final: public std::streambuf
{
public:
    BrotliOStreamBuf(std::streambuf* buffer, int compression_level=0, size_t buffer_size=BUFSIZ):
        m_sink(buffer),
        m_state(BrotliEncoderCreateInstance(nullptr, nullptr, nullptr)),
        m_write_area(buffer_size),
        m_compression_buffer(buffer_size)
    {
        initialize_stream(compression_level);

        // setup the write are buffer. Last byte is for the overflow operation
        setp(&m_write_area.front(), &m_write_area.front() + m_write_area.size() -1);
    }

    virtual ~BrotliOStreamBuf()
    {
        finalize_stream();
        BrotliEncoderDestroyInstance(m_state);
        m_state = nullptr;
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
        BrotliEncoderSetParameter(m_state, BROTLI_PARAM_QUALITY, compression_level);
    }

    void finalize_stream()
    {
        compress_buffer();

        size_t available_input = 0;
        size_t available_output = 0;
        size_t total_output = 0;

        // Read all the compressed data from the stream
        bool ok = BrotliEncoderCompressStream(
            m_state, BROTLI_OPERATION_FINISH,
            &available_input, nullptr,
            &available_output, nullptr, &total_output
        );

        if(!ok)
            throw std::runtime_error("Error during brotli compression");

        while(BrotliEncoderHasMoreOutput(m_state))
        {
            size_t output_size = 0;
            const uint8_t* available_output = BrotliEncoderTakeOutput(m_state, &output_size);

            m_sink->sputn(reinterpret_cast<const char*>(available_output), output_size);
        }
    }

    void compress_buffer()
    {
        const uint8_t* avalable_data = reinterpret_cast<const uint8_t*>(pbase());
        size_t available_input = std::distance(pbase(), pptr());

        size_t available_output = 0;
        size_t total_output = 0;

        // Ensure all the input data is written into the lzma stream encoder
        while(available_input > 0)
        {
            bool ok = BrotliEncoderCompressStream(
                m_state, BROTLI_OPERATION_PROCESS,
                &available_input, &avalable_data,
                &available_output, nullptr, &total_output
            );

            if(!ok)
                throw std::runtime_error("Error during brotli compression");

            while(BrotliEncoderHasMoreOutput(m_state))
            {
                size_t output_size = 0;
                const uint8_t* available_output = BrotliEncoderTakeOutput(m_state, &output_size);

                m_sink->sputn(reinterpret_cast<const char*>(available_output), output_size);
            }

        }

        setp(&m_write_area.front(), &m_write_area.front() + m_write_area.size() -1);
    }

private:
    std::streambuf* m_sink;
    BrotliEncoderState* m_state;
    std::vector<char> m_write_area;
    std::vector<char> m_compression_buffer;
};


class BrotliIStreamBuf final: public std::streambuf
{
public:
    BrotliIStreamBuf(std::streambuf* source, size_t internal_buffer_size=BUFSIZ):
        m_source(source),
        m_state(BrotliDecoderCreateInstance(nullptr, nullptr, nullptr)),
        m_src_buffer(internal_buffer_size),
        m_src_available(0),
        m_src_ptr(nullptr)
    {
        setg(nullptr, nullptr, nullptr);
    }

    virtual ~BrotliIStreamBuf()
    {
        BrotliDecoderDestroyInstance(m_state);
    }

protected:

    virtual int_type underflow() override
    {
        while(true)
        {
            if(m_src_available == 0)
            {
                m_src_available = m_source->sgetn(&m_src_buffer.front(), m_src_buffer.size());
                m_src_ptr = reinterpret_cast<const uint8_t*>(&m_src_buffer.front());
            }

            if(m_src_available == 0)
                return traits_type::eof();

            size_t available_output = 0;
            size_t total_output = 0;
            BrotliDecoderResult ret = BrotliDecoderDecompressStream(
                m_state,
                &m_src_available, &m_src_ptr,
                &available_output, nullptr, &total_output
            );

            if(ret == BROTLI_DECODER_RESULT_ERROR)
                throw std::runtime_error("Error during Brotli decompression");

            if(BrotliDecoderHasMoreOutput(m_state))
            {
                size_t output_size = 0;
                const uint8_t* brotli_buffer = BrotliDecoderTakeOutput(m_state, &output_size);
                char* read_area_ptr = reinterpret_cast<char*>(const_cast<uint8_t*>(brotli_buffer));
                setg(read_area_ptr, read_area_ptr, read_area_ptr + output_size);
                return traits_type::to_int_type(*gptr());
            }
            else
            {
                // No more data to decompress
                if(ret == BROTLI_DECODER_RESULT_SUCCESS)
                    return traits_type::eof();
            }
        }

        return traits_type::eof();
    }

private:
    std::streambuf* m_source;
    BrotliDecoderState* m_state;
    std::vector<char> m_src_buffer;
    size_t m_src_available;
    const uint8_t* m_src_ptr;
};


BrotliOStream::BrotliOStream(std::ostream& stream, int compression_level):
    std::ostream(new BrotliOStreamBuf(stream.rdbuf(), compression_level))
{}

BrotliOStream::~BrotliOStream()
{
    delete rdbuf();
}

std::vector<char> BrotliOStream::compress(const std::vector<char>& data, int compression_level)
{
    size_t encoded_max_size = BrotliEncoderMaxCompressedSize(data.size());
    encoded_max_size = encoded_max_size == 0 ? data.size() : encoded_max_size;

    std::vector<char> compressed_data(encoded_max_size);

    size_t compressed_size = compressed_data.size();
    bool ok = BrotliEncoderCompress(
        compression_level, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE,
        data.size(), reinterpret_cast<const uint8_t*>(data.data()),
        &compressed_size, reinterpret_cast<uint8_t*>(compressed_data.data())
    );

    if(!ok)
        throw std::runtime_error("Error during brotli compression");

    compressed_data.resize(compressed_size);

    return compressed_data;
}


BrotliIStream::BrotliIStream(std::istream& stream):
    std::istream(new BrotliIStreamBuf(stream.rdbuf()))
{}

BrotliIStream::~BrotliIStream()
{
    delete rdbuf();
}

std::vector<char> BrotliIStream::decompress(const std::vector<char>& data, size_t expected_size)
{
    expected_size = expected_size == 0 ? data.size() * 2 : expected_size;
    std::vector<char> decompressed_data(expected_size);

    size_t decompressed_size = decompressed_data.size();
    BrotliDecoderResult result = BrotliDecoderDecompress(
        data.size(), reinterpret_cast<const uint8_t*>(data.data()),
        &decompressed_size, reinterpret_cast<uint8_t*>(decompressed_data.data())
    );

    if(result != BROTLI_DECODER_RESULT_SUCCESS)
        throw std::runtime_error("Error during decompressing brotli data");

    decompressed_data.resize(decompressed_size);

    return decompressed_data;
}




} // namespace compressed_streams
