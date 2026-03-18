#include "st_usart.h"

namespace LBR
{
namespace Stmh7
{
StUsart::StUsart(USART_TypeDef* const base_addr, uint32_t sys_clck,
                 uint32_t baud_rate, OversampleMode mode)
    : base_addr(base_addr), mode(mode)
{
    if (mode == OversampleMode::BY_8)
    {
        usartdiv = (2 * sys_clck) / baud_rate;
    }
    else
    {
        usartdiv = sys_clck / baud_rate;
    }
}

bool StUsart::send(std::span<const uint8_t> txbuf)
{
    size_t count = 0;

    for (const auto& byte : txbuf)
    {
        // If TXE = 0 (not empty), stay in loop, if TXE = 1 (empty), exit loop.
        while (!(base_addr->ISR & USART_ISR_TXE_TXFNF))
        {
        }  // polling

        base_addr->TDR = byte;
        count++;
    }

    // if TC = 0, tx not complete, stay in loop. if TC = 1, tx complete, exit loop.
    while (!(base_addr->ISR & USART_ISR_TC))
    {
    }

    // if count is same as txbuf size, no data was lost, tx successful. Else, fail.
    return count == txbuf.size();
}

bool StUsart::receive(uint8_t& byte)
{

    /*
            When FIFO mode is disabled, the RXNE bit is set to indicate that the content of the
            shift register is transferred to the RDR. In other words, data have been received and
            can be read (as well as their associated error flags).
             */

    if (!(base_addr->ISR & USART_ISR_RXNE_RXFNE))
    {
        return false;
    }

    /*
            In single buffer mode:
            – When FIFO mode is disabled, clearing the RXNE flag is done by performing a software read 
            from the USART_RDR register. 
            */
    byte = base_addr->RDR;
    return true;
}

bool StUsart::init()
{
    if (base_addr == nullptr)
    {
        return false;
    }

    // Make sure USART is disabled (UE = 0) before doing anything
    base_addr->CR1 &= ~USART_CR1_UE;

    // Program the M bits in USART_CR1 to define the word length (8 data bits).
    base_addr->CR1 &= ~USART_CR1_M1;  // Set bit 28 (M1) to 0
    base_addr->CR1 &= ~USART_CR1_M0;  // Set bit 12 (M0) to 0

    // Select the desired baud rate using the USART_BRR register.
    // Per RM0468 section 53.5.7: when oversampling by 8, BRR is configured differently
    // BRR[15:4] = USARTDIV[15:4] (copied directly)
    // BRR[2:0]  = USARTDIV[3:0] >> 1 (shifted right, bit 0 discarded)
    // BRR[3]    = 0 (must be kept cleared, shift handles this naturally)
    if (mode == OversampleMode::BY_8)
    {
        uint16_t top = usartdiv & 0xFFF0;
        uint16_t bottom = (usartdiv & 0x000F) >> 1;
        base_addr->BRR = top | bottom;
    }
    else
    {
        base_addr->BRR = usartdiv;
    }

    // Program the number of stop bits in USART_CR2.
    // Set 1 stop bit [13:12] = 00
    base_addr->CR2 &= ~USART_CR2_STOP;

    // Enable the USART by writing the UE bit in USART_CR1 register to 1.
    base_addr->CR1 |= USART_CR1_UE;

    // Set the TE bit in USART_CR1 to send an idle frame as first transmission.
    base_addr->CR1 |= USART_CR1_TE;

    // Set the RE bit USART_CR1. This enables the receiver which begins searching for a start bit.
    base_addr->CR1 |= USART_CR1_RE;

    // Enable RXNE Interrupt
    base_addr->CR1 |= USART_CR1_RXNEIE_RXFNEIE;

    return true;
}

USART_TypeDef* StUsart::get_addr()
{
    return this->base_addr;
}

}  // namespace Stmh7
}  // namespace LBR