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
    LBR::Gpio* rx_pin;
    LBR::Gpio* tx_pin;
    USART_TypeDef* instance;
    uint16_t uartdiv;

public:
    void send_tx(const std::span<char> data) override;
    bool receive_rx(std::span<char>& data) override;
    void init(USART_TypeDef* instance, LBR::Gpio* tx, LBR::Gpio* rx);

    StUsart()
    {
    }
    StUsart(uint32_t sys_clck, uint32_t baud_rate)
    {
        uartdiv = sys_clck / baud_rate;
    }
};