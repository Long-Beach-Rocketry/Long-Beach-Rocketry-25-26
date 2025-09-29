#include "st_usart.h"

void StUsart::receive_rx(volatile uint8_t* data, size_t size)
{
    size_t count = 0;
    while (count < size)
    {
        instance->RDR = data[count++];
    }
}

void StUsart::send_tx(const uint8_t* data, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        while (!(instance->ISR & USART_ISR_TXE))
        {
        }
        instance->TDR = data[i];
    }

    while (!(instance->ISR & USART_ISR_TC))
    {
    }
}

void StUsart::init(USART_TypeDef* instance, LBR::Gpio* tx)
{
    StUsart::instance = instance;

    // StUsart::rx_pin = rx;
    StUsart::tx_pin = tx;

    StUsart::tx_pin->init();

    StUsart::instance->CR1 &= ~USART_CR1_UE;
    StUsart::instance->BRR = uartdiv;
    StUsart::instance->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}