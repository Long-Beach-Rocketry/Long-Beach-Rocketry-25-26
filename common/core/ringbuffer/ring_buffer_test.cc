
#include <gtest/gtest.h>

extern "C"
{
#include <cstdint>
#include "ringbuffer.h"
}

class RingBufferTest : public testing::Test
{
public:
    static constexpr size_t BUF_SIZE = 5;

    uint8_t data[BUF_SIZE];

    RingBuffer buf;
    RingBufferTest() : buf(data, BUF_SIZE){};
};

TEST_F(RingBufferTest, InsertAndPopTest)
{
    for (size_t i = 0; i < BUF_SIZE; ++i)
    {
        EXPECT_TRUE(buf.push(i));
    }

    EXPECT_FALSE(buf.push(0));

    uint8_t data = 0;
    for (size_t i = 0; i < BUF_SIZE; ++i)
    {
        EXPECT_TRUE(buf.pop(&data));
        EXPECT_EQ(i, data);
    }

    EXPECT_FALSE(buf.pop(&data));
}
