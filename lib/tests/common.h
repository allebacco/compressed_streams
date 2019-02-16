#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>

#include <gtest/gtest.h>


static std::vector<char> generate_data_range(const size_t count)
{
    std::vector<char> data(count);
    for(size_t i=0; i<count; ++i)
        data[i] = static_cast<char>(i & 0xFF);

    return data;
}

static constexpr size_t DATA_SIZE = 100000;
static const std::vector<char> DATA_REF = generate_data_range(DATA_SIZE);


template<typename OStreamType>
static std::string write_at_once_to_stream(const std::vector<char>& data)
{
    std::ostringstream ostream(std::ios_base::out|std::ios_base::binary);
    {
        OStreamType compressed_ostream(ostream);
        compressed_ostream.write(data.data(), data.size());
    }
    return ostream.str();
}


template<typename OStreamType>
static std::string write_one_byte_at_time_to_stream(const std::vector<char>& data)
{
    std::ostringstream ostream(std::ios_base::out|std::ios_base::binary);
    {
        OStreamType compressed_ostream(ostream);

        for(size_t i=0; i<data.size(); ++i)
            compressed_ostream.write(&data[i], 1);
    }

    return ostream.str();
}

template<typename OStreamType>
static std::string put_one_byte_at_time_to_stream(const std::vector<char>& data)
{
    std::ostringstream ostream(std::ios_base::out|std::ios_base::binary);
    {
        OStreamType compressed_ostream(ostream);

        for(size_t i=0; i<data.size(); ++i)
            compressed_ostream.put(data[i]);

        compressed_ostream.flush();
    }

    return ostream.str();
}

template<typename IStreamType>
static std::vector<char> read_at_once_from_stream(const std::string& serialized, const size_t original_size)
{
    std::vector<char> read_data(original_size);
    std::istringstream istream(serialized, std::ios_base::in|std::ios_base::binary);
    IStreamType compressed_istream(istream);
    compressed_istream.read(read_data.data(), read_data.size());

    return read_data;
}

template<typename IStreamType>
static std::vector<char> read_one_byte_at_time_from_stream(const std::string& serialized, const size_t original_size)
{
    std::vector<char> read_data(original_size);
    std::istringstream istream(serialized, std::ios_base::in|std::ios_base::binary);
    IStreamType compressed_istream(istream);

    for(size_t i=0; i<read_data.size(); ++i)
        compressed_istream.read(&read_data[i], 1);

    return read_data;
}

template<typename IStreamType>
static std::vector<char> get_one_byte_at_time_from_stream(const std::string& serialized, const size_t original_size)
{
    std::vector<char> read_data(original_size);
    std::istringstream istream(serialized, std::ios_base::in|std::ios_base::binary);
    IStreamType compressed_istream(istream);

    for(size_t i=0; i<read_data.size(); ++i)
        read_data[i] = compressed_istream.get();

    return read_data;
}





template<typename IOStreamTypes>
class IOStreamTest : public ::testing::Test
{
public:

};

TYPED_TEST_SUITE_P(IOStreamTest);


TYPED_TEST_P(IOStreamTest, WriteAllAtOnce)
{
    using OStreamType = typename std::tuple_element<0, TypeParam>::type;
    using IStreamType = typename std::tuple_element<1, TypeParam>::type;

    std::string serialized = write_at_once_to_stream<OStreamType>(DATA_REF);
    std::vector<char> test_data = read_at_once_from_stream<IStreamType>(serialized, DATA_REF.size());

    EXPECT_EQ(DATA_REF, test_data);
}


TYPED_TEST_P(IOStreamTest, WriteOneByteAtATime)
{
    using OStreamType = typename std::tuple_element<0, TypeParam>::type;
    using IStreamType = typename std::tuple_element<1, TypeParam>::type;

    std::string serialized = write_one_byte_at_time_to_stream<OStreamType>(DATA_REF);
    std::vector<char> test_data = read_at_once_from_stream<IStreamType>(serialized, DATA_REF.size());

    EXPECT_EQ(DATA_REF, test_data);
}


TYPED_TEST_P(IOStreamTest, PutOneByteAtATime)
{
    using OStreamType = typename std::tuple_element<0, TypeParam>::type;
    using IStreamType = typename std::tuple_element<1, TypeParam>::type;

    std::string serialized = put_one_byte_at_time_to_stream<OStreamType>(DATA_REF);
    std::vector<char> test_data = read_at_once_from_stream<IStreamType>(serialized, DATA_REF.size());

    EXPECT_EQ(DATA_REF, test_data);
}


TYPED_TEST_P(IOStreamTest, ReadOneByteAtATime)
{
    using OStreamType = typename std::tuple_element<0, TypeParam>::type;
    using IStreamType = typename std::tuple_element<1, TypeParam>::type;

    std::string serialized = write_at_once_to_stream<OStreamType>(DATA_REF);
    std::vector<char> test_data = read_one_byte_at_time_from_stream<IStreamType>(serialized, DATA_REF.size());

    EXPECT_EQ(DATA_REF, test_data);
}


TYPED_TEST_P(IOStreamTest, GetOneByteAtATime)
{
    using OStreamType = typename std::tuple_element<0, TypeParam>::type;
    using IStreamType = typename std::tuple_element<1, TypeParam>::type;

    std::string serialized = write_at_once_to_stream<OStreamType>(DATA_REF);
    std::vector<char> test_data = get_one_byte_at_time_from_stream<IStreamType>(serialized, DATA_REF.size());

    EXPECT_EQ(DATA_REF, test_data);
}


REGISTER_TYPED_TEST_SUITE_P(IOStreamTest,
                                WriteAllAtOnce,
                                WriteOneByteAtATime,
                                PutOneByteAtATime,
                                ReadOneByteAtATime,
                                GetOneByteAtATime);

#endif // COMMON_H
