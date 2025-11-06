#include "st_usart.h"

StUsart::StUsart(USART_TypeDef* base_addr, uint32_t sys_clck,
                 uint32_t baud_rate)
    : base_addr(base_addr), uartdiv(sys_clck / baud_rate) {};

bool StUsart::receive_rx(std::span<char> data)
{
    static size_t index = 0;

    //prevent overrun
    if (base_addr->ISR & USART_ISR_ORE)
    {
        base_addr->ICR |= USART_ICR_ORECF;
    }

    if ((base_addr->ISR & USART_ISR_RXNE) && index <= data.size())
    {
        data[index] = base_addr->RDR;

        if (data[index] == '\n' || data[index] == '\r' || index > data.size())
        {
            index = 0;
            return true;
        }
        index++;
    }

    return false;
}

bool StUsart::send_tx(const std::span<char> data)
{
    for (auto byte : data)
    {

        while (!(base_addr->ISR & USART_ISR_TXE))
        {
        }
        if (byte == '\n' || byte == '\r')
        {
            return false;
        }
        base_addr->TDR = static_cast<uint16_t>(byte);
    }

    while (!(base_addr->ISR & USART_ISR_TC))
    {
    }
    return true;
}

void StUsart::init()
{

    StUsart::base_addr->CR1 &= ~USART_CR1_UE;
    StUsart::base_addr->BRR = uartdiv;
    StUsart::base_addr->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
    StUsart::base_addr->CR1 |= USART_CR1_RXNEIE;
}
