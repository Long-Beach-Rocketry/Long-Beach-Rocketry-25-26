/**
* @file usart_pipe.h
* @author Bex Saw
* @brief Framer for board-to-board communication over USART→RS485→USART.
*        Frame format: [SOF:4][LEN:1][PAYLOAD:N][CRC32:4][EOF:4]
* @version 0.3
*/
#pragma once

#include <array>
#include <cstdint>
#include <span>
#include "crc.h"
#include "pb_cmd.h"
#include "ring_buffer.h"
#include "rs485.h"
#include "usart.h"

namespace LBR
{
class Pipeline
{

public:
    explicit Pipeline(Crc& crc, Rs485& rs485);

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

    /**
    * @brief Push one received byte into the RX ring buffer.
    *        Call this from the USART RX interrupt handler so incoming bytes are
    *        captured automatically without polling. receive() then decodes whole
    *        frames out of the buffer the ISR has filled.
    * @param byte The byte read from the USART data register in the ISR.
    */
    void push_rx(uint8_t byte);

    /**
    * @brief Total length (LEN + overhead) of the most recently transmitted frame.
    * @return kFrameOverhead + payload_len for the last successful send(), or 0 if none yet.
    */
    uint16_t get_tx_frame_len() const
    {
        return tx_frame_len;
    }

    /**
    * @brief Total length (LEN + overhead) of the most recently received frame.
    * @return kFrameOverhead + payload_len for the last successfully decoded frame, or 0 if none yet.
    */
    uint16_t get_rx_frame_len() const
    {
        return rx_frame_len;
    }

private:
    // Format of the frame: [SOF:4][LEN:1][PAYLOAD:N][CRC32:4][EOF:4]
    static constexpr uint32_t kSof{0xAB6B0BAA};
    static constexpr uint32_t kEof{0x67676767};
    static constexpr uint8_t kSofLen{4};
    static constexpr uint8_t kHeaderLen{kSofLen + 1};  // SOF + LEN byte
    static constexpr uint8_t kCrcLen{4};
    static constexpr uint8_t kEofLen{4};
    static constexpr uint16_t kBufSize{256};
    static constexpr uint8_t kFrameOverhead = kHeaderLen + kCrcLen + kEofLen;
    static constexpr uint16_t kMaxPayloadLen = kBufSize - kFrameOverhead;

    Crc& crc;
    Rs485& rs485;
    RingBuffer<uint8_t, kBufSize> rx_buffer;
    std::array<uint8_t, kBufSize> tx_buffer;
    uint16_t tx_frame_len{0};
    uint16_t rx_frame_len{0};

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
