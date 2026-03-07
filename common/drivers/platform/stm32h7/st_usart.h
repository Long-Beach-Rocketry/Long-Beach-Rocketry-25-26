/**
 * @brief This file is implementing hardware specific functions defined in the generic USART class.
 */

#pragma once // Include guard to prevent multiple inclusions of this header file

#include "stm32h723xx.h" // Include the correct header for the specific STM32H7 device being used
#include "usart.h" // Include the generic USART class header

namespace LBR // Use the LBR namespace to avoid naming conflicts
{
namespace Stmh7 // Use the Stmh7 namespace for STM32H7 specific implementations
{
class StUsart : public Usart 
{
private:
    USART_TypeDef* base_addr; // Pointer to the USART peripheral's base address
    uint16_t usartdiv; // Variable to hold the calculated USARTDIV value for baud rate configuration

public:
    /**
     * @brief Parameterized constructor which initializes important values for specific USART object.
     * 
     * @param base_addr The base address of the USART peripheral to be used.
     * @param sys_clck The specific system clock frequency of hardware.
     * @param baud_rate The chosen baud rate to send and receive data on a serial monitor
     */
    StUsart(USART_TypeDef* base_addr, uint32_t sys_clck, uint32_t baud_rate);

    /**
     * @brief sends data to serial output.
     * 
     * @param txbuf An uint8_t std::array data to be sent
     * @return True on success. False, otherwise.
     */
    bool send(std::span<const uint8_t> txbuf) override;

    /**
     * @brief Receives data from serial input.
     * 
     * @param byte Reference to store received byte
     * @return True if byte was received, false if no data available
     */
    bool receive(uint8_t& byte) override;

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

};
} // namespace Stmh7
} // namespace LBR