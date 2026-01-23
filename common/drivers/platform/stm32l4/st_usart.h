/**
 * @brief This file is implementing hardware specific functions defined in the generic USART class.
 */

#pragma once

#include "stm32l476xx.h"
#include "usart.h"

namespace LBR
{
namespace Stml4
{
class StUsart : public Usart
{
private:
    USART_TypeDef* base_addr;
    uint16_t uartdiv;

public:
    /**
     * @brief Paramterized contructor which initilazes important values for specific USART object.
     * 
     * @param sys_clck The specific system clock frequency of hardware.
     * @param baud_rate The chosen baud rate to send and recieve data on a serial monitor.
     */

    StUsart(USART_TypeDef* base_addr, uint32_t sys_clck, uint32_t baud_rate);

    /**
     * @brief Sends data to serial output.
     * 
     * @param txbuf An uint8_t std::array data to be sent
     * @param size Variable of size_t type which specifies the size of message.
     */
    bool send_tx(std::span<const uint8_t> txbuf) override;

    /**
     * @brief Recieves data from serial input.
     * 
     * @param byte Reference to store received byte
     * @return True if byte was received, false if no data available
     */
    bool receive_rx(uint8_t& byte) override;

    /**
     * @brief Initializes the USART and associated Rx and Tx pins.
     * 
     * @return True successful initializaiton. False otherwise.
     */
    bool init();

    /**
     * @brief Get the base_addr of the UART object
     * 
     * @return USART_TypeDef* 
     */
    USART_TypeDef* get_addr();
};
}  // namespace Stml4
}  // namespace LBR