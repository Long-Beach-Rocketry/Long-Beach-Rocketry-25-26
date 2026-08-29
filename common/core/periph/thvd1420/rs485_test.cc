#include "rs485.h"
#include <gtest/gtest.h>

namespace LBR
{
namespace Test
{

/**
 * @brief A mock implementation of the Gpio interface 
 * perfectly matched to the pure virtual signatures in gpio.h.
 */
class MockGpio : public LBR::Gpio
{
public:
    MockGpio() = default;
    virtual ~MockGpio() = default;

    // Matches: virtual bool set(const bool active) = 0;
    bool set(const bool active) override
    {
        last_value = active;
        set_called_count++;
        return true;
    }

    // Matches: virtual bool read() = 0;
    bool read() override
    {
        return last_value;
    }

    // Matches: virtual bool toggle() = 0;
    bool toggle() override
    {
        last_value = !last_value;
        return true;
    }

    /**
     * @brief Current logical state of the mock pin (true = HIGH, false = LOW).
     * Tracks virtual voltage state to verify driver direction modes.
     */
    bool last_value = false;

    /**
     * @brief Total number of times set() has been called on this pin.
     * Used to verify that it doesn't execute redundant hardware writes.
     */
    uint8_t set_called_count = 0;
};

/**
 * @brief Test fixture mirroring the layout structure of RingBufferTest
 */
class Rs485Test : public testing::Test
{
protected:
    // Initialize the driver by binding it to our mock references
    Rs485Test() : driver(mock_de, mock_re)
    {
    }

    MockGpio mock_de;
    MockGpio mock_re;
    LBR::Rs485 driver;
};

/**
 * @brief Verify that init() implicitly forces the transceiver into RECEIVE mode
 */
TEST_F(Rs485Test, InitializationDefaultsToReceive)
{
    ASSERT_TRUE(driver.init())
        << "Driver failed to execute initial state routine.";

    // In RECEIVE mode: DE = 0 (Disable TX), RE = 0 (Enable RX)
    EXPECT_EQ(mock_de.last_value, 0)
        << "Driver Enable (DE) pin should be cleared.";
    EXPECT_EQ(mock_re.last_value, 0)
        << "Receiver Enable (RE) pin should be cleared (active-LOW).";
}

/**
 * @brief Verify correct pin tracking transitions when changing to TRANSMIT mode
 */
TEST_F(Rs485Test, SetDirectionTransmit)
{
    ASSERT_TRUE(driver.set_direction(LBR::Rs485::Direction::TRANSMIT));

    // In TRANSMIT mode: DE = 1 (Enable TX), RE = 1 (Disable RX)
    EXPECT_EQ(mock_de.last_value, 1)
        << "Driver Enable (DE) pin was not asserted HIGH.";
    EXPECT_EQ(mock_re.last_value, 1)
        << "Receiver Enable (RE) pin was not pulled HIGH to disable RX loop.";
}

/**
 * @brief Verify correct pin tracking transitions when changing to LOOPBACK mode
 */
TEST_F(Rs485Test, SetDirectionLoopback)
{
    ASSERT_TRUE(driver.set_direction(LBR::Rs485::Direction::LOOPBACK));

    // In LOOPBACK mode: DE = 1 (Enable TX), RE = 0 (Enable RX simultaneously)
    EXPECT_EQ(mock_de.last_value, 1)
        << "Driver Enable (DE) pin was not asserted HIGH.";
    EXPECT_EQ(mock_re.last_value, 0) << "Receiver Enable (RE) pin should be "
                                        "dropped LOW to allow echo loops.";
}

/**
 * @brief Verify back-to-back state transitions execute completely without data caching issues
 */
TEST_F(Rs485Test, SequentialStateTransitions)
{
    // Cycle through states sequentially to verify individual toggling works reliably
    EXPECT_TRUE(driver.set_direction(LBR::Rs485::Direction::TRANSMIT));
    EXPECT_EQ(mock_de.last_value, 1);
    EXPECT_EQ(mock_re.last_value, 1);

    EXPECT_TRUE(driver.set_direction(LBR::Rs485::Direction::RECEIVE));
    EXPECT_EQ(mock_de.last_value, 0);
    EXPECT_EQ(mock_re.last_value, 0);

    // Verify that the set method was hit exactly 6 times across the transitions
    EXPECT_EQ(mock_de.set_called_count, 2);
    EXPECT_EQ(mock_re.set_called_count, 2);
}

}  // namespace Test
}  // namespace LBR