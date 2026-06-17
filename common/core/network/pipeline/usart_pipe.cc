#include "usart_pipe.h"

namespace LBR
{

bool Pipeline::send(const PbCmd* msg, Usart& usart)
{
    if (msg == nullptr)
        return false;

    /* Format of the frame: [SOF:4][LEN:1][PAYLOAD:N][CRC32:4][EOF:4] */

    // Encode protobuf payload after the header slot
    int payload_len =
        msg->encode(tx_buffer.data() + kHeaderLen, kMaxPayloadLen);
    if (payload_len <= 0)
        return false;

    // Build header — SOF big-endian, then LEN byte
    tx_buffer[0] = static_cast<uint8_t>(kSof >> 24);
    tx_buffer[1] = static_cast<uint8_t>(kSof >> 16);
    tx_buffer[2] = static_cast<uint8_t>(kSof >> 8);
    tx_buffer[3] = static_cast<uint8_t>(kSof);
    tx_buffer[kSofLen] = static_cast<uint8_t>(payload_len);

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

    // Append EOF big-endian
    tx_buffer[crc_offset + kCrcLen + 0] = static_cast<uint8_t>(kEof >> 24);
    tx_buffer[crc_offset + kCrcLen + 1] = static_cast<uint8_t>(kEof >> 16);
    tx_buffer[crc_offset + kCrcLen + 2] = static_cast<uint8_t>(kEof >> 8);
    tx_buffer[crc_offset + kCrcLen + 3] = static_cast<uint8_t>(kEof);

    size_t frame_len = kFrameOverhead + static_cast<size_t>(payload_len);

    return usart.send(std::span<const uint8_t>(tx_buffer.data(), frame_len));
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
    // Slide one byte at a time until the 4-byte SOF pattern is at the head
    while (rx_buffer.size() >= kSofLen)
    {
        uint8_t b0 = 0, b1 = 0, b2 = 0, b3 = 0;
        if (!rx_buffer.peek(0, b0) || !rx_buffer.peek(1, b1) ||
            !rx_buffer.peek(2, b2) || !rx_buffer.peek(3, b3))
            return false;
        uint32_t candidate = (static_cast<uint32_t>(b0) << 24) |
                             (static_cast<uint32_t>(b1) << 16) |
                             (static_cast<uint32_t>(b2) << 8) |
                             static_cast<uint32_t>(b3);
        if (candidate == kSof)
            break;
        rx_buffer.pop(b0);
    }

    // We need at least enough bytes for the header to know the payload length
    if (rx_buffer.size() < kHeaderLen)
        return false;

    // LEN byte sits immediately after the 4-byte SOF
    uint8_t len_byte = 0;
    rx_buffer.peek(kSofLen, len_byte);
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

    // Consume the frame from the ring buffer
    for (size_t i = 0; i < frame_len; i++)
    {
        uint8_t dummy = 0;
        rx_buffer.pop(dummy);
    }

    // Decode the message if it's true that we got here with a valid frame
    return msg->decode(frame.data() + kHeaderLen, payload_len);
}

}  // namespace LBR
