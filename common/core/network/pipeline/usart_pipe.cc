#include "usart_pipe.h"

namespace LBR
{

bool Pipeline::send(const PbCmd* msg, Usart& usart)
{
    if (msg == nullptr)
        return false;

    /* Format of the frame: [SOF:1][LEN:1][PAYLOAD:N][CRC32:4][EOF:1] */

    // Encode protobuf payload after the header slot
    int payload_len =
        msg->encode(tx_buffer.data() + kHeaderLen, kMaxPayloadLen);
    if (payload_len <= 0)
        return false;

    // Build header
    tx_buffer[0] = kSof;
    tx_buffer[1] = static_cast<uint8_t>(payload_len);

    // Compute CRC32 over [SOF][LEN][PAYLOAD]
    // Note: we do CRC32 b/c the payload is variable length
    // and we want to avoid copying it into a separate buffer for crc calculation

    size_t crc_input_len = kHeaderLen + static_cast<size_t>(payload_len);
    uint32_t crc_val = 0;
    if (!crc.compute(std::span<const uint8_t>(tx_buffer.data(), crc_input_len),
                     crc_val))
    {
        return false;
    }

    // Append CRC32 little-endian after payload
    size_t crc_offset = crc_input_len;
    tx_buffer[crc_offset + 0] = static_cast<uint8_t>(crc_val);
    tx_buffer[crc_offset + 1] = static_cast<uint8_t>(crc_val >> 8);
    tx_buffer[crc_offset + 2] = static_cast<uint8_t>(crc_val >> 16);
    tx_buffer[crc_offset + 3] = static_cast<uint8_t>(crc_val >> 24);

    // Append EOF
    tx_buffer[crc_offset + kCrcLen] = kEof;

    size_t frame_len = kFrameOverhead + static_cast<size_t>(payload_len);

    return usart.send(std::span<const uint8_t>(tx_buffer.data(), frame_len));
}

void Pipeline::poll_usart(Usart& usart)
{
    uint8_t byte = 0;

    // Poll the USART for new bytes and push them into the RX ring buffer
    while (usart.receive(byte))
    {
        rx_buffer.push(byte);
    }
}

bool Pipeline::process_frame(PbCmd* msg)
{
    // Drop bytes before SOF
    while (!rx_buffer.empty())
    {
        uint8_t candidate = 0;
        if (!rx_buffer.peek(0, candidate))
            return false;
        if (candidate == kSof)
            break;
        rx_buffer.pop(candidate);
    }

    // We need at least enough bytes for the header to know the payload length
    if (rx_buffer.size() < kHeaderLen)
        return false;

    // Peek length byte to determine full frame size before processing
    uint8_t len_byte = 0;
    rx_buffer.peek(1, len_byte);
    size_t payload_len = static_cast<size_t>(len_byte);

    // Reject payloads that would exceed our buffer
    if (payload_len > kMaxPayloadLen)
    {
        uint8_t dummy = 0;
        rx_buffer.pop(dummy);
        return false;
    }

    // We need the full frame (header + payload + crc + eof) to proceed
    size_t frame_len = kFrameOverhead + payload_len;
    if (rx_buffer.size() < frame_len)
        return false;

    // Peek full frame into a linear buffer for validation and decode
    std::array<uint8_t, kBufSize> frame{};
    for (size_t i = 0; i < frame_len; i++)
    {
        rx_buffer.peek(i, frame[i]);
    }

    // Validate EOF
    if (frame[frame_len - 1] != kEof)
    {
        for (size_t i = 0; i < frame_len; i++)
        {
            uint8_t dummy = 0;
            rx_buffer.pop(dummy);
        }
        return false;
    }

    // Validate CRC32 over [SOF][LEN][PAYLOAD]
    size_t crc_offset = kHeaderLen + payload_len;
    uint32_t received_crc =
        static_cast<uint32_t>(frame[crc_offset]) |
        (static_cast<uint32_t>(frame[crc_offset + 1]) << 8) |
        (static_cast<uint32_t>(frame[crc_offset + 2]) << 16) |
        (static_cast<uint32_t>(frame[crc_offset + 3]) << 24);

    // Compare the CRC32 in the frame with a freshly computed CRC32 over the header+payload
    if (!crc.compare(std::span<const uint8_t>(frame.data(), crc_offset),
                     received_crc))
    {
        for (size_t i = 0; i < frame_len; i++)
        {
            uint8_t dummy = 0;
            rx_buffer.pop(dummy);
        }
        return false;
    }

    // Consume the validated frame from the ring buffer
    for (size_t i = 0; i < frame_len; i++)
    {
        uint8_t dummy = 0;
        rx_buffer.pop(dummy);
    }

    // Decode the message if it's true that we got here with a valid frame
    return msg->decode(frame.data() + kHeaderLen, payload_len);
}

bool Pipeline::receive(PbCmd* msg, Usart& usart)
{
    if (msg == nullptr)
        return false;

    // Poll the usart for new bytes from the ring buffer
    poll_usart(usart);

    // Process the rx buffer to extract and validate a complete frame, then decode the protobuf message
    return process_frame(msg);
}

}  // namespace LBR
