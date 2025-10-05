#include "st_usart.h"

StUsart::StUsart(uint32_t sys_clck, uint32_t baud_rate)
    : uartdiv(sys_clck / baud_rate) {};

bool StUsart::receive_rx(std::span<char>& data)
{
    size_t size = sizeof(instance->RDR);
    size_t count = 0;
    while (count < size)
    {
        while (!(instance->ISR & USART_ISR_RXNE))
        {
        }
        data[count++] = instance->RDR;
    }

    return true;
}

void StUsart::send_tx(const std::span<char> data)
{
    for (auto byte : data)
    {

        while (!(instance->ISR & USART_ISR_TXE))
        {
        }
        instance->TDR = static_cast<uint8_t>(byte);
    }

    while (!(instance->ISR & USART_ISR_TC))
    {
    }
}

void StUsart::init(USART_TypeDef* instance)
{
    StUsart::instance = instance;

    StUsart::instance->CR1 &= ~USART_CR1_UE;
    StUsart::instance->BRR = uartdiv;
    StUsart::instance->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
    StUsart::instance->CR1 |= USART_CR1_RXNEIE;
}
