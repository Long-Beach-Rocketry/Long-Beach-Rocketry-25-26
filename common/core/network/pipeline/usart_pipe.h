/**
* @file cmd_msg.h
* @author Bex Saw
* @brief Helper functions for sending and receiving protobuf messages over USART-RS485-USART communication between the Board-To-Board.
* @version 0.1
*/
#pragma once

#include <array>
#include <cstddef>
#include <span>
#include "pb_cmd.h"
#include "usart.h"

/// TODO: Need to add RS485 (transceiver module) where it taking the bytestream from USART and send it to the Telemetry board's USART.

/**
* @brief At the @send: It's throw the raw data + struct messages into a buffer encode into a payload and adding data frame of ID, Length, Payload, Checksum. 
*           Then send the buffer over USART to RS485 to USART of Telemetry board. The data frame should be like this: [ID][Length][Payload][Checksum]
*        id: 1 byte, length: 1 byte, payload: variable length (up to 128 bytes or more), crc: 1 byte 
*
*        At the @recieve: It's recieve the data from the RS485-TO-USART RX. 
*        Then pass that into a recieve buffer and then they will be statemachine 
*        abstract over the buffer to validate frame throw in ring buffer and then 
*        throw in the SX module to bytestream LoRa
*      
*/

namespace LBR
{
class Pipeline
{
public:
    /**
    * @brief Helper funciton to send a CmdMessage over USART. 
    * @note Board-To-TelemBoard communication is USART-RS485-USART 
    *       *RS485 is a bus so it will take the bytestream from USART and send it to the Telemetry board's USART. 
    *        The Telemetry board will then decode the bytestream to get the original message.
    * @param msg The command message object to send.
    * @param usart The USART interface to use for sending the protobuf message.
    */
    bool send(const PbCmd* msg, Usart& usart);

    /**
    * @brief Helper function to receive a CmdMessage over USART. 
    * @note TelemBoard-communication is USART-RS485-USART 
    *       *RS485 is a bus so it will take the bytestream from USART and send it to the Telemetry board's USART. 
    *        The Telemetry board will then decode the bytestream to get the original message.
    * @param msg The command message object to fill with received data.
    * @param usart The USART interface to use for receiving the protobuf message.
    */
    bool receive(PbCmd* msg, Usart& usart);

private:
    // SX module receive buffer at 254 bytes of [ID][Length][Payload][Checksum]
    std::array<uint8_t, 256> rx_buffer;
    std::array<uint8_t, 256> tx_buffer;
};
}  // namespace LBR
