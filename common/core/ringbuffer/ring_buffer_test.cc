
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

    ringbuffer buf;
    RingBufferTest() : buf(data, BUF_SIZE){};
};

TEST_F(RingBufferTest, InsertAndPopTest)
{
    for (size_t i = 0; i < BUF_SIZE; ++i)
    {
        EXPECT_TRUE(buf.ringbuffer_push(i));
    }

    EXPECT_FALSE(buf.ringbuffer_push(0));

    uint8_t data = 0;
    for (size_t i = 0; i < BUF_SIZE; ++i)
    {
        EXPECT_TRUE(buf.ringbuffer_pop(&data));
        EXPECT_EQ(i, data);
    }

    EXPECT_FALSE(buf.ringbuffer_pop(&data));
}
