/**
* @file usart_pipe.h
* @author Bex Saw
* @brief Framer for board-to-board communication over USART→RS485→USART.
*        Frame format: [SOF:4][LEN:1][PAYLOAD:N][CRC32:4][EOF:4]
* @version 0.3
*/
#pragma once

#include <array>
#include <cstddef>
#include <span>
#include "crc.h"
#include "pb_cmd.h"
#include "ring_buffer.h"
#include "usart.h"

namespace LBR
{
class Pipeline
{
public:
    explicit Pipeline(Crc& crc) : crc(crc)
    {
    }

    /**
    * @brief Sends a protobuf message over USART with framing and CRC.
    * @param msg Pointer to the PbCmd message to send.
    * @param usart Reference to the Usart instance.
    * @return True if the message was sent successfully, false otherwise.
    */
    bool send(const PbCmd* msg, Usart& usart);

    /**
    * @brief Receives a protobuf message over USART with framing and CRC.
    * @param msg Pointer to the PbCmd message to receive.
    * @param usart Reference to the Usart instance.
    * @return True if a valid message was received, false otherwise.
    */
    bool receive(PbCmd* msg, Usart& usart);

private:
    // Format of the frame: [SOF:4][LEN:1][PAYLOAD:N][CRC32:4][EOF:4]
    static constexpr uint32_t kSof{0xAB6B0BAA};
    static constexpr uint32_t kEof{0x67676767};
    static constexpr size_t kSofLen{4};
    static constexpr size_t kHeaderLen{kSofLen + 1};  // SOF + LEN byte
    static constexpr size_t kCrcLen{4};
    static constexpr size_t kEofLen{4};
    static constexpr size_t kBufSize{256};
    static constexpr size_t kFrameOverhead = kHeaderLen + kCrcLen + kEofLen;
    static constexpr size_t kMaxPayloadLen = kBufSize - kFrameOverhead;

    Crc& crc;
    RingBuffer<uint8_t, kBufSize> rx_buffer;
    std::array<uint8_t, kBufSize> tx_buffer;

    /**
    * @brief Poll the USART for incoming bytes and push them into the RX ring buffer.
    * @param usart Reference to the Usart instance to poll.
    */
    void poll_usart(Usart& usart);

    /**
    * @brief Process the RX buffer to extract and validate a complete frame, then decode the protobuf message.
    * @param msg Pointer to the PbCmd message to populate with the decoded data.
    * @return True if a valid frame was processed and decoded, false otherwise.
    */
    bool process_frame(PbCmd* msg);
};
}  // namespace LBR
