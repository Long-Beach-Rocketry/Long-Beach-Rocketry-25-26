/**
 * @brief This file is implementing hardware specific functions defined in the generic USART class.
 */

#pragma once

#include <cstddef>
#include <span>
#include <string>
#include "gpio.h"
#include "stm32l476xx.h"
#include "usart.h"

struct StUsartParams
{
    USART_TypeDef* base_addr;
    uint32_t sys_clck;
    uint32_t baud_rate;
};

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
    explicit StUsart(USART_TypeDef* base_addr, uint32_t sys_clck,
                     uint32_t baud_rate);

    /**
     * @brief Sends data to serial output.
     * 
     * @param data A uint8_t pointer that contains message to be sent.
     * @param size Variable of size_t type which specifies the size of message.
     */
    void send_tx(const uint8_t* data, size_t size) override;

    /**
     * @brief Recieves data from serial input.
     * 
     * @param data A uint8_t pointer that holds data to be recieved from serial input.
     */
    uint8_t receive_rx(uint8_t* data) override;
    /**
     * @brief Initializes the USART and associated Rx and Tx pins.
     * 
     */
    void init();
};