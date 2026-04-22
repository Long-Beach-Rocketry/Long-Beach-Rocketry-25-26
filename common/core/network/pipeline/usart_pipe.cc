#include "usart_pipe.h"

namespace LBR
{

bool Pipeline::send(const PbCmd* msg, Usart& usart)
{
    // Messages should not exceed buffer size
    int len = msg->encode(tx_buffer.data(), tx_buffer.size());

    if (len <= 0)
    {
        return false;
    }

    // Send the encoded message over USART to RS485
    usart.send(std::span<const uint8_t>(tx_buffer.data(), len));

    // TODO: Add RS485 transmission logic here send to Telemetry board's USART.

    return true;
}

bool Pipeline::receive(PbCmd* msg, Usart& usart)
{
    // TODO: Add RS485 reception logic here to receive from Telemetry board's USART and fill rx_buffer.

    // Read bytes from USART until we have a complete payload size (frame)
    size_t count = 0;

    for (size_t i = 0; i < rx_buffer.size(); ++i)
    {
        uint8_t byte;
        // Try to receive a byte from USART. If no more data is available, break the loop.
        if (usart.receive(byte))
        {
            // Store the received byte in the buffer and increment the count
            rx_buffer[count++] = byte;
        }
        else
        {
            break;
        }
    }

    // If we received less than a full frame, return false
    if (count < rx_buffer.size())
        return false;

    return true;
}

}  // namespace LBR
