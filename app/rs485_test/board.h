/**
 * @file board.h
 * @brief Board-specific definitions and architecture contract for the isolated rs485_test application.
 * @author Alex Pulido
 * @date 06/27/2026
 */

#pragma once
#include "gpio.h"
#include "rs485.h"
#include "usart.h"

namespace LBR
{
/**
 * @struct Board
 * @brief Architecture wrapper aggregating system dependencies for the hardware test rig.
 */
struct Board
{
    Usart& usart; /**< Reference to bare-metal serial controller. */
    Gpio& rx;     /**< Reference to underlying UART RX pin hardware resource. */
    Gpio& tx;     /**< Reference to underlying UART TX pin hardware resource. */
    Rs485&
        rs485; /**< Reference to the platform driver handling independent DE and /RE lines. */
};

/**
 * @brief Initializes isolated clocks, pin states, and registers for target hardware.
 * @return True on peripheral setup success, false otherwise.
 */
bool bsp_init();

/**
 * @brief Singleton getter granting application space tracking access to physical board definitions.
 * @return Reference to the concrete Board allocation.
 */
Board& get_board();

}  // namespace LBR