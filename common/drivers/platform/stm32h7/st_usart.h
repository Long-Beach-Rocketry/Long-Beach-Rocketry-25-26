/**
 * @brief This file is implementing hardware specific functions defined in the generic USART class.
 * 
 * @note Current implementation uses polling for TX/RX with FIFO disabled.
 *      Future optimizations may include:
 *      - DMA driven TX/RX to free CPU for navigation, IMU, and control tasks
 *      - FIFO mode to reduce interrupt frequency and improve throughput
 */

#pragma once  // Include guard to prevent multiple inclusions of this header file

#include "stm32h723xx.h"  // Include the correct header for the specific STM32H7 device being used
#include "usart.h"  // Include the generic USART class header

namespace LBR  // Use the LBR namespace to avoid naming conflicts
{
namespace Stmh7  // Use the Stmh7 namespace for STM32H7 specific implementations
{
class StUsart : public Usart
{
public:
    /**
     * @brief Public enum class that determines which oversampling mode will be used by the driver.
     * 
     * @note BY_16 provides better noise tolerance (default).
     * @note BY_8 doubles the maximum achievable baud rate. 
     */
    enum class OversampleMode
    {
        BY_16,
        BY_8
    };

    /**
     * @brief Parameterized constructor which initializes important values for specific USART object.
     * 
     * @param params_ Struct containing USART configuration parameters:
     *                - base_addr: base address of the USART peripheral to be used
     *                - sys_clck: specific system clock frequency of hardware
     *                - baud_rate: chosen baud rate to send and receive data on a serial monitor
     *                - mode: chosen oversampling mode, default is by 16
     */
    struct StUsartParams
    {
        USART_TypeDef* base_addr;
        uint32_t sys_clck;
        uint32_t baud_rate;
        OversampleMode mode = OversampleMode::BY_16;
    };
    StUsart(const StUsartParams& params_);

    /**
     * @brief sends data to serial output.
     * 
     * @param txbuf An uint8_t std::array data to be sent.
     * @return True on success. False, otherwise.
     */
    bool send(std::span<const uint8_t> txbuf) override;

    /**
     * @brief Receives data from serial input.
     * 
     * @param byte Reference to store received byte.
     * @return True if byte was received, false if no data available.
     */
    bool receive(uint8_t& byte) override;
    // Note: why is this received in a form of byte/8 bits rather than a span/array?

    /**
     * @brief Initializes the USART and associated Rx and Tx pins.
     * 
     * @return True successful initialization. False otherwise.
     */
    bool init();

    /**
     * @brief Get the base_addr of the UART object
     * 
     * @return USART_TypeDef* 
     */
    USART_TypeDef* get_addr();

private:
    USART_TypeDef* const
        base_addr;  // Pointer to the USART peripheral's base address
    uint16_t
        usartdiv;  // Variable to hold the calculated USARTDIV value for baud rate configuration
    OversampleMode mode;  // Variable to hold oversample mode
};
}  // namespace Stmh7
}  // namespace LBR