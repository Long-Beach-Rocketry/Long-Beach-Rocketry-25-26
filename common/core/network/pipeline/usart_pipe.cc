#include "usart_pipe.h"

namespace LBR
{

/// SEQUENCE: 1. Encode the CmdMessage into a byte buffer using the encode function of PbCmd.
///           2. Send the encoded byte buffer over USART to RS485.
///           3. RS485 will then transmit the byte buffer to the Telemetry board's USART.
bool Pipeline::send(const PbCmd* msg, Usart& usart)
{
    // Messages should not exceed buffer size
    int payload_len = msg->encode(tx_buffer.data(), tx_buffer.size());

    if (payload_len <= 0)
    {
        return false;
    }

    // Frame ID
    tx_buffer[0] = 0x67;
    tx_buffer[1] = static_cast<int>(payload_len);

    // Add CRC

    // Combine all of the frame + length + frame + crc into a single buffer to send over USART to RS485.

    // int final_msg = (frame & length) + payload_len + crc_len;

    // Send! the encoded message over USART to RS485
    usart.send(std::span<const uint8_t>(tx_buffer.data(), final_msg));

    // TODO: Add RS485 transmission logic here send to Telemetry board's USART.

    return true;
}

/// SEQUENCE: 1. Receive bytes from RS485 then to UART into rx_buffer until no more data or buffer is full.
///           2. Once we have data in rx_buffer, we can then process it to SX module so it can be sent over LoRa.
///           ** At this logic it's only Recieve to Telemetry board's USART. We will add another logic on top of that when sending to SX module or within this function to process it.
bool Pipeline::receive(PbCmd* msg, Usart& usart)
{
    // TODO: Add RS485 reception logic here to receive from Telemetry board's USART and fill rx_buffer.

    // Read bytes from USART until we receive all available data or fill the buffer
    size_t count = 0;

    uint8_t byte;
    // Try to receive a byte from USART. Loop until no more data or buffer is full.
    while (usart.receive(byte))
    {
        // Push the received byte into the ring buffer
        if (rx_buffer.push(byte))
        {
            count++;
        }
        else
        {
            // Ring buffer is full
            break;
        }
    }

    // We now have data in our ring buffer.
    // We just check if we have received *anything* for now.
    if (count == 0)
        return false;

    return true;
}

}  // namespace LBR
