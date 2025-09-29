/**
 * @brief This file is implementing hardware specific functions defined in the generic USART class.
 */

#pragma once

#include <cstddef>
<<<<<<< HEAD
#include <span>
#include <string>
=======
>>>>>>> 6b9398d (usart transmit)
#include "gpio.h"
#include "stm32l476xx.h"
#include "usart.h"

class StUsart : public Usart
{
private:
<<<<<<< HEAD
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
    bool receive_rx(uint8_t* data) override;

    /**
     * @brief Initializes the USART and associated Rx and Tx pins.
     * 
     * @param instannce The USART base_addr that is to be currently used.
     */
    void init();
=======
    // Gpio* rx_pin;
    LBR::Gpio* tx_pin;
    USART_TypeDef* instance;
    uint16_t uartdiv;

public:
    void send_tx(const uint8_t* data, size_t size) override;
    void receive_rx(volatile uint8_t* data, size_t size) override;
    void init(USART_TypeDef* instance, LBR::Gpio* tx);

    StUsart()
    {
    }
    StUsart(uint32_t sys_clck, uint32_t baud_rate)
    {
        uartdiv = sys_clck / baud_rate;
    }
>>>>>>> 6b9398d (usart transmit)
};