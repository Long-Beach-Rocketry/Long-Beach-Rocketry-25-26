#include "ring_buffer.h"
#include <gtest/gtest.h>

class RingBufferTest : public testing::Test
{
protected:
    RingBufferTest() = default;
    uint8_t rx;
    LBR::RingBuffer<uint8_t, 4> rb;
};

/**
 * @brief Verify correct behavior for an empty ring buffer
 * 
 */
TEST_F(RingBufferTest, EmptyRingBuffer)
{
    ASSERT_TRUE(rb.empty()) << "The ring buffer is not empty.";
    EXPECT_FALSE(rb.full());
    EXPECT_EQ(rb.size(), 0);
    EXPECT_EQ(rb.capacity(), 4);
    EXPECT_TRUE(rb.reset());
    EXPECT_FALSE(rb.pop(rx));

    rb.push(6);
    EXPECT_EQ(rb.get_write(), 1);
    EXPECT_EQ(rb.size(), 1) << "Could not add an element to the ring buffer.";
    rb.pop(rx);
    EXPECT_EQ(rb.get_read(), 1);
    EXPECT_EQ(rx, 6) << "Wrong value was read.";
    EXPECT_EQ(rb.size(), 0)
        << "Current ring buffer size after read is incorrect.";
}

/**
 * @brief Verify correct behavior for a half filled ring buffer
 * 
 */
TEST_F(RingBufferTest, HalfFilledRingBuffer)
{
    rb.push(6);
    rb.push(7);
    ASSERT_FALSE(rb.empty()) << "The ring buffer is empty.";
    ASSERT_FALSE(rb.full()) << "The ring buffer is full";
    ASSERT_EQ(rb.size(), 2)
        << "Elements 6 and/or 7 were not properly added to the buffer.";
    ASSERT_EQ(rb.get_write(), 2) << "Write pointer not at expected indice.";

    rb.pop(rx);
    EXPECT_EQ(rx, 6) << "Incorrect value was read.";
    EXPECT_EQ(rb.get_read(), 1) << "Read pointer not incremented properly";
    EXPECT_EQ(rb.size(), 1) << "Count was not decremented properly.";

    rb.pop(rx);
    EXPECT_EQ(rx, 7) << "Incorrect value was read.";
    EXPECT_EQ(rb.get_read(), 2) << "Read pointer not incremented properly";
    EXPECT_EQ(rb.size(), 0) << "Count was not decremented properly.";
    EXPECT_EQ(rb.get_read(), rb.get_write())
        << "Read and write pointer do not match";
}

/**
 * @brief Verify correct behavior for a full ring buffer
 * 
 */
TEST_F(RingBufferTest, FullRingBuffer)
{
    // Add 4 values to the ring buffer
    rb.push(6);
    rb.push(7);
    rb.push(8);
    rb.push(9);
    ASSERT_TRUE(rb.full()) << "The ring buffer is not full.";
    ASSERT_EQ(rb.size(), 4) << "Count is incorrect for a full rb.";
    ASSERT_EQ(rb.get_write(), 0) << "Write pointer in unexpected indice.";

    // Read and remove those 4 values from the ring buffer
    rb.pop(rx);
    EXPECT_EQ(rx, 6) << "Expected value 6.";
    EXPECT_EQ(rb.get_read(), 1) << "Expected read at position 1.";
    rb.pop(rx);
    EXPECT_EQ(rx, 7) << "Expected value 7.";
    EXPECT_EQ(rb.get_read(), 2) << "Expected read at position 2.";
    rb.pop(rx);
    EXPECT_EQ(rx, 8) << "Expected value 8.";
    EXPECT_EQ(rb.get_read(), 3) << "Expected read at position 3.";
    rb.pop(rx);
    EXPECT_EQ(rx, 9) << "Expected value 9.";
    EXPECT_EQ(rb.get_read(), 0) << "Expected read at position 0.";
    EXPECT_EQ(rb.size(), 0) << "Count was not 0.";
}

/**
 * @brief Verify correct behavior for a NO_OVERWRITE push for a full ring buffer
 * 
 */
TEST_F(RingBufferTest, NoOverwriteFullRingBuffer)
{
    // Add 4 values to the ring buffer
    rb.push(6);
    rb.push(7);
    rb.push(8);
    rb.push(9);
    ASSERT_TRUE(rb.full()) << "Ring buffer is not full.";
    ASSERT_EQ(rb.get_write(), rb.get_read())
        << "Read and write pointers do not match.";

    // Do a no overwrite push on a full buffer
    ASSERT_FALSE(rb.push(10)) << "Unexpected data overwrite at indice 0.";

    // Read the data to confirm
    rb.pop(rx);
    EXPECT_EQ(rx, 6) << "Expected value 6 not read.";
}

/**
 * @brief Verify correct behavior for a OVERWRITE push for a full ring buffer
 * 
 */
TEST_F(RingBufferTest, OverwriteFullRingBuffer)
{
    // Add 4 values to the ring buffer
    rb.push(6);
    rb.push(7);
    rb.push(8);
    rb.push(9);
    ASSERT_TRUE(rb.full()) << "Ring buffer is not full.";
    ASSERT_EQ(rb.get_write(), rb.get_read())
        << "Read and write pointers do not match.";

    // Do a overwrite push on a full buffer
    ASSERT_TRUE(
        rb.push(10, LBR::RingBuffer<uint8_t, 4>::WritePolicy::OVERWRITE))
        << "Data was not overwritten.";
    EXPECT_EQ(rb.get_read(), 1);
    EXPECT_EQ(rb.get_write(), 1);
    EXPECT_TRUE(rb.full());

    for (int i = 0; i < 4; i++)
    {
        rb.pop(rx);
    }
    EXPECT_EQ(rx, 10);
    EXPECT_EQ(rb.get_read(), 1);
    EXPECT_TRUE(rb.empty());
}