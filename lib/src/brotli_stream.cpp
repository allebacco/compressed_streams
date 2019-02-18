
#include "../include/compressed_streams/brotli_stream.h"

#include <brotli/encode.h>
#include <brotli/decode.h>


namespace compressed_streams
{

BrotliOStream::BrotliOStream(std::ostream& stream, int compression_level)
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

BrotliIStream::BrotliIStream(std::istream& stream)
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
