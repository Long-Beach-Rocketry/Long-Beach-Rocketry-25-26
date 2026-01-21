#include "st_usart.h"

StUsart::StUsart(USART_TypeDef* base_addr, uint32_t sys_clck,
                 uint32_t baud_rate)
    : base_addr(base_addr), uartdiv(sys_clck / baud_rate) {};

bool StUsart::receive_rx(std::span<uint8_t> rxbuf)
{
    //prevent overrun
    if (base_addr->ISR & USART_ISR_ORE)
    {
        base_addr->ICR |= USART_ICR_ORECF;
    }

    for (auto& byte : rxbuf)
    {
        while (!(base_addr->ISR & USART_ISR_RXNE))
        {
            // Add a timeout condition and return false if flag isn't cleared in time
        }

        byte = base_addr->RDR;
    }

    return true;
}

bool StUsart::receive_rx_nb(uint8_t& byte)
{
    // Check if data is available (non-blocking)
    if (!(base_addr->ISR & USART_ISR_RXNE))
    {
        return false;
    }

    // Clear overrun error if present
    if (base_addr->ISR & USART_ISR_ORE)
    {
        base_addr->ICR |= USART_ICR_ORECF;
    }

    // Read the byte
    byte = base_addr->RDR;
    return true;
}

bool StUsart::send_tx(std::span<const uint8_t> txbuf)
{
    size_t count = 0;
    for (const auto& byte : txbuf)
    {
        while (!(base_addr->ISR & USART_ISR_TXE))
        {
        }
        base_addr->TDR = byte;
        count++;
    }

    while (!(base_addr->ISR & USART_ISR_TC))
    {
    }

    return (count == txbuf.size());
}

bool StUsart::init()
{

    if (StUsart::base_addr == nullptr)
    {
        return false;
    }
    StUsart::base_addr->CR1 &= ~USART_CR1_UE;

    // Set baud rate register
    StUsart::base_addr->BRR = uartdiv;

    // Enable RX, TX, and UART
    StUsart::base_addr->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    // Enable RXNE Interrupt
    StUsart::base_addr->CR1 |= USART_CR1_RXNEIE;
    return true;
}
