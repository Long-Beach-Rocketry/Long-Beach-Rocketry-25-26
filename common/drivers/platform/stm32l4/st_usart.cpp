#include "st_usart.h"

void StUsart::receive_rx(volatile uint8_t* data, size_t size)
{
    size_t count = 0;
    while (count < size)
    {
        instance->RDR = data[count++];
    }
}

void StUsart::send_tx(uint8_t* data, size_t size)
{
    size_t count = 0;
    while (count < size)
    {
        instance->TDR = data[count++];
    }
}

void StUsart::init(USART_TypeDef* instance)
{
    StUsart::instance = instance;

    // StUsart::rx_pin = rx;
    // StUsart::tx_pin = tx;
    // rx_pin->configure();
    // tx_pin->configure();

    instance->CR1 &= ~USART_CR1_UE;
    instance->CR1 |= USART_CR1_RE;
    instance->CR1 |= USART_CR1_TE;
    instance->CR1 &= ~USART_CR1_M0;
    instance->CR1 &= ~USART_CR1_M1;
    instance->BRR = (((uartdiv / 16) << USART_BRR_DIV_MANTISSA_Pos) |
                     ((uartdiv % 16) << USART_BRR_DIV_FRACTION_Pos));
    instance->CR1 |= USART_CR1_UE;
}