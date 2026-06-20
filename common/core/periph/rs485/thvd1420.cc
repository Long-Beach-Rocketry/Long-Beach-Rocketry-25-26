#include "thvd1420.h"

namespace LBR::Stmh7
{

Thvd1420::Thvd1420(const RS485Params& params)
    : base_addr(params.base_addr), baud_rate(params.baud_rate)
{
}

bool Thvd1420::init(uint32_t peripheral_clock_freq)
{
    if (base_addr == nullptr)
    {
        return false;
    }

    // Make sure USART is disabled (UE = 0) before doing anything
    base_addr->CR1 &= ~USART_CR1_UE;

    // Calculate BRR using the live clock speed passed from rs485 test main
    uint32_t brr_value = peripheral_clock_freq / baud_rate;
    base_addr->BRR = brr_value;

    // Setup 8-bit word length, disable parity, enable Transmitter and Receiver
    base_addr->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0 | USART_CR1_PCE);
    base_addr->CR1 |= (USART_CR1_TE | USART_CR1_RE);

    // RS-485 SPECIFIC: Configure Driver Enable Assertion/De-assertion times
    // Adds a vital hardware clock-cycle guard band surrounding your bytes
    base_addr->CR1 &= ~(USART_CR1_DEAT | USART_CR1_DEDT);
    base_addr->CR1 |=
        (0x01 << USART_CR1_DEAT_Pos);  // Sample 1-bit period assertion buffer
    base_addr->CR1 |=
        (0x01
         << USART_CR1_DEDT_Pos);  // Sample 1-bit period de-assertion buffer

    // Configure for standard 1 Stop Bit
    base_addr->CR2 &= ~USART_CR2_STOP;

    // Natively automate the THVD1420 Half-Duplex line via hardware
    base_addr->CR3 |= USART_CR3_DEM;  // Enable Hardware Driver Enable Mode
    base_addr->CR3 &=
        ~USART_CR3_DEP;  // Driver Enable Polarity: Active High (DE = 1 to talk)

    // Re-enabling the USART block
    base_addr->CR1 |= USART_CR1_UE;

    return true;
}

bool Thvd1420::send(std::span<const uint8_t> txbuf)
{
    for (const uint8_t byte : txbuf)
    {
        // Wait until the Transmit Data Register has space for the next payload byte
        while (!(base_addr->ISR & USART_ISR_TXE_TXFNF));

        // Pass the element directly to the data pipeline
        base_addr->TDR = byte;
    }

    // CRITICAL STEPS FOR HALF-DUPLEX:
    // Poll the Transmission Complete (TC) flag. This forces the microcontroller to stall
    // until the last bit of the Stop bit has cleared the internal output shift register.
    // This allows hardware to release the DE pin down without data truncation.
    while (!(base_addr->ISR & USART_ISR_TC));

    return true;
}

bool Thvd1420::receive(uint8_t& byte)
{
    // Check if the Read Data Register Not Empty flag is set
    if (base_addr->ISR & USART_ISR_RXNE_RXFNE)
    {
        byte = static_cast<uint8_t>(base_addr->RDR);
        return true;
    }

    return false;
}

}  // namespace LBR::Stmh7