#include "st_usart.h"

StUsart::StUsart(USART_TypeDef* base_addr, uint32_t sys_clck,
                 uint32_t baud_rate)
    : base_addr(base_addr), uartdiv(sys_clck / baud_rate) {};

bool StUsart::receive_rx(std::span<char> data)
{
    for (auto& c : data)
    {
        while (!(base_addr->ISR & USART_ISR_RXNE))
        {
        }

        c = base_addr->RDR;
    }

    return true;
}

void StUsart::send_tx(const std::span<char> data)
{
    for (auto byte : data)
    {

        while (!(base_addr->ISR & USART_ISR_TXE))
        {
        }
        base_addr->TDR = static_cast<uint16_t>(byte);
    }

    while (!(base_addr->ISR & USART_ISR_TC))
    {
    }
}

void StUsart::init()
{

    StUsart::base_addr->CR1 &= ~USART_CR1_UE;
    StUsart::base_addr->BRR = uartdiv;
    StUsart::base_addr->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
    StUsart::base_addr->CR1 |= USART_CR1_RXNEIE;
}
