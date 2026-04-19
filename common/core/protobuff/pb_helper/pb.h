/**
* @file usart_pb_helper.h
* @author Bex Saw
* @brief Helper functions for sending and receiving protobuf command messages over USART.
* @version 0.1
*/
#pragma once
#include <cstddef>
#include <span>
#include "cmd_msg.h"
#include "usart.h"

namespace LBR
{

/**
* @brief Helper function to send a CmdMessage over USART. Returns true on success, false on failure.
* @param msg The command message to send.
* @param usart The USART interface to use for sending.
*/
bool send_cmd_msg(const CmdMessage* msg, Usart& usart);

/**
* @brief Helper function to receive a CmdMessage over USART. Returns true on success, false on failure.
* @param msg The command message object to fill with received data.
* @param usart The USART interface to use for receiving.
* @param maxlen The maximum length of the message to receive (default 128 bytes).
*/
bool receive_cmd_msg(CmdMessage* msg, Usart& usart, size_t maxlen = 128);

}  // namespace LBR
