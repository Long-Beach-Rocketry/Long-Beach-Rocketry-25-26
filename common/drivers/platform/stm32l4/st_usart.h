#pragma once

// #include "gpio.h"
#include <stdbool.h>
#include <cstddef>
#include "gpio.h"
#include "stm32l476xx.h"
#include "usart.h"

class StUsart : public Usart
{
private:
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
};