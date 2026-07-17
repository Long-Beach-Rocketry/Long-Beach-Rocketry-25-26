/**
 * @file rs485.h
 * @brief Transceiver direction controller driver for separate DE and /RE configurations.
 * @author Alex Pulido
 * @date 06/27/2026
 */

#pragma once
#include <cstdint>
#include "gpio.h"

namespace LBR
{
class Rs485
{
public:
    enum class Direction : uint8_t
    {
        RECEIVE = 0,  /**< DE=0 (Disable TX), RE=0 (Enable RX). */
        TRANSMIT = 1, /**< DE=1 (Enable TX), RE=1 (Disable RX). */
        LOOPBACK = 2  /**< DE=1 (Enable TX), RE=0 (Enable RX simultaneously). */
    };

    /**
     * @brief Constructor binding the driver to separate DE and RE pins.
     * @param de_pin GPIO pin controlling Driver Enable (Active HIGH).
     * @param re_pin GPIO pin controlling Receiver Enable (Active LOW).
     */
    Rs485(Gpio& de_pin, Gpio& re_pin) : de_pin(de_pin), re_pin(re_pin)
    {
    }

    ~Rs485() = default;

    /**
     * @brief Safely initializes the pins into default listening state.
     */
    bool init()
    {
        return set_direction(Direction::RECEIVE);
    }

    /**
     * @brief Controls the hardware lines independently based on target mode.
     */
    bool set_direction(Direction dir)
    {
        bool success = true;

        if (dir == Direction::TRANSMIT)
        {
            success &= de_pin.set(1);  // Turn Transmitter ON
            success &=
                re_pin.set(1);  // Turn Receiver OFF (RE high disables it)
        }
        else if (dir == Direction::LOOPBACK)
        {
            success &= de_pin.set(1);  // Turn Transmitter ON
            success &= re_pin.set(0);  // Turn Receiver ON (RE low enables it)
        }
        else  // Default to RECEIVE
        {
            success &= de_pin.set(0);  // Turn Transmitter OFF
            success &= re_pin.set(0);  // Turn Receiver ON
        }

        return success;
    }

private:
    Gpio& de_pin; /**< Reference to physical DE pin (Active HIGH). */
    Gpio& re_pin; /**< Reference to physical RE pin (Active LOW). */
};
}  // namespace LBR