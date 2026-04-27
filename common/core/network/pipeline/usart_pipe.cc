#include "usart_pipe.h"

/// TODO: SIMPLIFY THIS CODE! This is just a rough draft to get the logic down.

namespace LBR
{

/// SEQUENCE: 1. Encode the CmdMessage into a byte buffer using the encode function of PbCmd.
///           2. Send the encoded byte buffer over USART to RS485.
///           3. RS485 will then transmit the byte buffer to the Telemetry board's USART.
bool Pipeline::send(const PbCmd* msg, Usart& usart)
{
    // Check if the message is valid before encoding
    if (msg == nullptr)
    {
        return false;
    }

    uint8_t* payload_start = tx_buffer.data() + kHeaderLen;
    size_t payload_capacity = tx_buffer.size() - kHeaderLen - kCrcLen;

    // Messages should not exceed buffer size
    int payload_len = msg->encode(tx_buffer.data(), tx_buffer.size());

    if (payload_len <= 0 || static_cast<size_t>(payload_len) > payload_capacity)
    {
        return false;
    }

    tx_buffer[0] = kFrameId;
    tx_buffer[1] = static_cast<uint8_t>(payload_len);

    // Add CRC

    // Combine all of the frame + length + frame + crc into a single buffer to send over USART to RS485.

    // int final_msg = (frame & length) + payload_len + crc_len;

    // Enable RS485 High (Transmit mode)

    // Send! the encoded message over USART to RS485
    usart.send(std::span<const uint8_t>(tx_buffer.data(), final_msg));

    // TODO: Add RS485 transmission logic here send to Telemetry board's USART.

    // After sending, we can disable RS485 High (Transmit mode) to allow it to receive again.

    return true;
}

/// SEQUENCE: 1. Receive bytes from RS485 then to UART into rx_buffer until no more data or buffer is full.
///           2. Once we have data in rx_buffer, we can then process it to SX module so it can be sent over LoRa.
///           ** At this logic it's only Recieve to Telemetry board's USART. We will add another logic on top of that when sending to SX module or within this function to process it.
bool Pipeline::receive(PbCmd* msg, Usart& usart)
{
    if (msg == nullptr)
    {
        return false;
    }

    // TODO: Add RS485 reception logic here to receive from Telemetry board's USART and fill rx_buffer.

    // Read bytes from USART until we receive all available data or fill the buffer
    size_t count = 0;

    // 1. Pull all available USART bytes into ring buffer
    uint8_t byte = 0;

    while (usart.receive(byte))
    {
        if (!rx_buffer.push(byte))
        {
            // RX ring buffer full
            return false;
        }
    }

    // Look for frame ID & drop junk bytes until we find it
    while (rx_buffer.size() > 0)
    {
        uint8_t candidate = 0;

        if (!rx_buffer.peek(0, candidate))
        {
            return false;
        }

        if (candidate == kFrameId)
        {
            break;
        }

        // Drop junk byte until frame ID is found
        rx_buffer.pop(candidate);
    }

    // Need header after cleanup
    if (rx_buffer.size() < kHeaderLen)
    {
        return false;
    }

    // We have frame ID, so we can read the payload length
    uint8_t frame_id = 0;
    uint8_t payload_len_u8 = 0;

    // Peek frame ID and payload length without popping them yet
    rx_buffer.peek(0, frame_id);
    rx_buffer.peek(1, payload_len_u8);

    // Validate frame ID
    if (frame_id != kFrameId)
    {
        return false;
    }

    // Calculate total expected frame length (header + payload + CRC)
    size_t payload_len = static_cast<size_t>(payload_len_u8);
    size_t full_frame_len = kHeaderLen + payload_len + kCrcLen;

    // Wait until we have recieved the full frame
    if (rx_buffer.size() < full_frame_len)
    {
        // Not enough data for a full frame yet
        return false;
    }

    // Copy full frame from ring buffer into linear temp buffer

    // Extract received CRC

    // CRC covers frame id + payload length + payload

    // Calculate CRC of received frame and compare to extracted CRC

    // If CRC matches, send it to SX module for LoRa transmission

    return true;
}
}  // namespace LBR
