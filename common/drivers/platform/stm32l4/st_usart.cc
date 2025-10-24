#include "st_usart.h"

StUsart::StUsart(USART_TypeDef* base_addr, uint32_t sys_clck,
                 uint32_t baud_rate)
    : base_addr(base_addr), uartdiv(sys_clck / baud_rate) {};

bool StUsart::receive_rx(uint8_t* data, size_t size)
{
    size_t count = 0;
    while (count < size)
    {
        while (!(base_addr->ISR & USART_ISR_RXNE))
        {
        }
        data[count++] = base_addr->RDR;
    }

    return true;
}

void StUsart::send_tx(const uint8_t* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {

        while (!(base_addr->ISR & USART_ISR_TXE))
        {
        }
        base_addr->TDR = data[i];
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
