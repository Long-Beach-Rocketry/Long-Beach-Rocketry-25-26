/**
 * @brief THVD1420 RS485 Transceiver Driver using STM32H7 Hardware DE.
 */

#pragma once

#include "stm32h723xx.h"
#include "usart.h"

namespace LBR
{
namespace Stmh7
{

/**
 * @brief Configuration parameter block for initializing the RS485 peripheral.
 */
struct RS485Params
{
    USART_TypeDef*
        base_addr;  // Base address of the STM32H7 hardware peripheral (e.g., USART3)
    uint32_t baud_rate;
};

class Thvd1420 : public Usart
{
public:
    /**
     * @brief Constructor injects the bare-metal peripheral config parameters.
     * @param params Configuration structure passed by constant reference.
     */
    explicit Thvd1420(const RS485Params& params);

    /**
     * @brief Initializes the low-level registers for RS485 Hardware Driver Enable mode.
     * @return true on successful setup, false if configuration registers fail validation.
     */
    bool init(uint32_t peripheral_clock_freq);

    /**
     * @brief Sends a block of data using a span pattern.
     * Hardware DE automatically asserts/de-asserts the THVD1420 line.
     * @param txbuf Span of read-only bytes to transmit.
     * @return true on successful completion of the shift register clear.
     */
    bool send(std::span<const uint8_t> txbuf) override;

    /**
     * @brief Checks the RXNE flag and grabs a byte from the receive register.
     * @param byte Reference to store the received byte.
     * @return true if data was ready and captured successfully, false if no data available.
     */
    bool receive(uint8_t& byte) override;

private:
    USART_TypeDef* const base_addr;  // Pointer to bare-metal hardware registers
    uint32_t baud_rate;              // Saved target baud rate
};

}  // namespace Stmh7
}  // namespace LBR