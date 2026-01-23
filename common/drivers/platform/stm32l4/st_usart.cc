#include "st_usart.h"

namespace LBR
{
namespace Stml4
{
StUsart::StUsart(USART_TypeDef* base_addr, uint32_t sys_clck,
                 uint32_t baud_rate)
    : base_addr(base_addr), uartdiv(sys_clck / baud_rate)
{
}

bool StUsart::Receive(uint8_t& byte)
{
    // Clear overrun error if present
    if (base_addr->ISR & USART_ISR_ORE)
    {
        base_addr->ICR |= USART_ICR_ORECF;
    }

    // Read the byte
    byte = base_addr->RDR;
    return true;
}

bool StUsart::Send(std::span<const uint8_t> txbuf)
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
    // Clear USART enable bit
    StUsart::base_addr->CR1 &= ~USART_CR1_UE;

    // Define USART word length: clear M1 and M0 for 8 data bits
    StUsart::base_addr->CR1 &= ~USART_CR1_M;

    // Set baud rate register
    StUsart::base_addr->BRR = uartdiv;

    // Set 1 stop bit
    StUsart::base_addr->CR2 &= ~USART_CR2_STOP;

    // Enable RX, TX, and UART
    StUsart::base_addr->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    // Enable RXNE Interrupt
    StUsart::base_addr->CR1 |= USART_CR1_RXNEIE;
    return true;
}

USART_TypeDef* StUsart::GetAddr()
{
    return this->base_addr;
}
}  // namespace Stml4
}  // namespace LBR