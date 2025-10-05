/**
 * @brief This file is implementing hardware specific functions defined in the generic USART class.
 */

#pragma once

#include <stdbool.h>
#include <cstddef>
#include <span>
#include <string>
#include "gpio.h"
#include "stm32l476xx.h"
#include "usart.h"

class StUsart : public Usart
{
private:
    USART_TypeDef* instance;
    uint16_t uartdiv;

public:
    /**
     * @brief Paramterized contructor which initilazes important values for specific USART object.
     * 
     * @param sys_clck The specific system clock frequency of hardware.
     * @param baud_rate The chosen baud rate to send and recieve data on a serial monitor.
     */
    explicit StUsart(uint32_t sys_clck, uint32_t baud_rate);

    /**
     * @brief Sends data to serial output.
     * 
     * @param data A variable of string type that contains message to be sent.
     */
    void send_tx(const std::span<char> data) override;

    /**
     * @brief Recieves data from serial input.
     * 
     * @param data A variable of string type that holds data to be recieved from serial input.
     */
    bool receive_rx(std::span<char>& data) override;

    /**
     * @brief Initializes the USART and associated Rx and Tx pins.
     * 
     * @param instannce The USART instance that is to be currently used.
     */
    void init(USART_TypeDef* instance);
};