/**
 * @file rs485.h
 * @brief Transceiver direction controller driver for half-duplex RS-485 chips.
 * @author Alex Pulido
 * @date 6/20/2026
 */

#pragma once
#include "gpio.h"

namespace LBR
{
/**
 * @class Rs485
 * @brief Driver instance to manage the physical direction pin of an RS-485 transceiver.
 *
 * Driver wraps a single GPIO output pin tied to the DE (Driver Enable) 
 * and RE (Receiver Enable) pins of a transceiver chip.
 */
class Rs485
{
public:
    /**
     * @enum Direction
     * @brief Determines whether the transceiver is actively talking or listening on the bus.
     */
    enum class Direction
    {
        RECEIVE = 0,
        TRANSMIT = 1
    };

    /**
     * @brief Parameterized constructor binding the driver to a physical pin interface.
     * @param dir_pin Reference to the generic Gpio interface controlling direction.
     */
    explicit Rs485(Gpio& dir_pin) : dir_pin(dir_pin)
    {
    }

    /**
     * @brief Default destructor.
     */
    ~Rs485() = default;

    /**
     * @brief Initializes the driver pin and forces it into a safe receive mode.
     * @return True if the underlying GPIO initialized successfully, false otherwise.
     */
    bool init()
    {
        return set_direction(Direction::RECEIVE);
    }

    /**
     * @brief Toggles the physical DE and RE lines to switch half-duplex modes.
     * @param dir The target operational mode (RECEIVE or TRANSMIT).
     * @return True if the underlying GPIO write succeeded, false otherwise.
     */
    bool set_direction(Direction dir)
    {
        if (dir == Direction::TRANSMIT)
        {
            // DE = 1, RE = 1
            return dir_pin.set(1);
        }
        else
        {
            // DE = 0, RE = 0
            return dir_pin.set(0);
        }
    }

private:
    Gpio& dir_pin;
};
}  // namespace LBR