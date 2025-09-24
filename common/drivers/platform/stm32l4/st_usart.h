#pragma once

// #include "gpio.h"
#include <cstddef>
#include "stm32l476xx.h"
#include "usart.h"

class StUsart : public Usart
{
private:
    // Gpio* rx_pin;
    // Gpio* tx_pin;
    USART_TypeDef* instance;
    uint16_t uartdiv;

public:
    void send_tx(uint8_t* data, size_t size) override;
    void receive_rx(volatile uint8_t* data, size_t size) override;
    void fake_func();

    StUsart()
    {
    }
    StUsart(USART_TypeDef* instance, int sys_clck, int baud_rate)
    {
        StUsart::instance = instance;
        // StUsart::rx_pin = rx;
        // StUsart::tx_pin = tx;
        // rx_pin->configure();
        // tx_pin->configure();

        uartdiv = sys_clck / baud_rate;
        instance->CR1 &= ~USART_CR1_UE;
        instance->CR1 |= USART_CR1_RE;
        instance->CR1 |= USART_CR1_TE;
        instance->CR1 &= ~USART_CR1_M0;
        instance->CR1 &= ~USART_CR1_M1;
        instance->BRR = (((uartdiv / 16) << USART_BRR_DIV_MANTISSA_Pos) |
                         ((uartdiv % 16) << USART_BRR_DIV_FRACTION_Pos));
        instance->CR1 |= USART_CR1_UE;
    }
};