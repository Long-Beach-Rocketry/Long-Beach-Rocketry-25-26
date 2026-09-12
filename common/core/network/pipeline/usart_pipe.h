/**
* @file usart_pipe.h
* @author Bex Saw
* @brief Framer for board-to-board communication over USART→RS485→USART.
*        NEW: Frame format: [SOF:4][LEN:1][PAYLOAD:kMaxPayloadLen, zero-padded][CRC32:4][EOF:4]
*        Every frame is the same fixed size on the wire (so a future LoRa hop
*        can rely on a constant packet size); LEN still carries the real
*        encoded length so decode() knows how many payload bytes are real.
* 
*        OLD: Frame format: [SOF:4][LEN:1][PAYLOAD:N][CRC32:4][EOF:4]
*        Every frame is variable size on the wire, so a future LoRa hop would
*        need to know the real payload length to know how many bytes to read.
* @version 0.4
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
    * @brief Pull one validated raw frame out of the RX ring buffer without decoding
    *        it, for boards that just relay bytes onward (e.g. to the SX1262 LoRa
    *        driver) instead of consuming the protobuf message locally. Framing,
    *        length, and CRC/EOF are still checked here so a desync on the RS485
    *        hop doesn't burn LoRa airtime forwarding garbage.
    * @param out_frame Buffer to copy the raw frame into (SOF..EOF, fixed frame size).
    * @param out_len Set to the number of bytes copied into out_frame.
    * @return True if a valid frame was found in the RX ring buffer and copied.
    */
    bool receive_raw(std::span<uint8_t> out_frame, uint16_t& out_len);

    /**
    * @brief Push one received byte into the RX ring buffer.
    *        Call this from the USART RX interrupt handler so incoming bytes are
    *        captured automatically without polling. receive() then decodes whole
    *        frames out of the buffer the ISR has filled.
    * @param byte The byte read from the USART data register in the ISR.
    */
    void push_rx(uint8_t byte);

    /**
    * @brief Total length of the most recently transmitted frame.
    */
    uint16_t get_tx_frame_len() const
    {
        return tx_frame_len;
    }

    /**
    * @brief Total length of the most recently received frame.
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
    static constexpr uint16_t kPayloadLen = kBufSize - kFrameOverhead;

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

    /**
    * @brief Slide the RX ring buffer to the next SOF, validate LEN/CRC/EOF, and
    *        copy the raw fixed-size frame out. Shared by process_frame() (which
    *        decodes the result) and receive_raw() (which forwards it as-is).
    * @param frame Buffer to receive the raw frame bytes (SOF..EOF).
    * @param frame_len Set to the total frame length copied into frame.
    * @param payload_len Set to the real encoded payload length from the LEN byte.
    * @return True if a valid frame was found, validated, and consumed from rx_buffer.
    */
    bool extract_frame(std::array<uint8_t, kBufSize>& frame,
                       uint16_t& frame_len, uint8_t& payload_len);
};
}  // namespace LBR
