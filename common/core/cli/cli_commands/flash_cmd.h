/**
 * @file flash_cmd.h
 * @author Aaron Oh
 * @brief The header for the blink command module
 */

#pragma once
#include <span>
#include "cmd.h"

namespace LBR
{

class Gpio;

namespace FlashCmd
{
extern const Cmd flashCmd;

/**
 * @brief Initializes the blink command with used gpio leds. Up to three for Stm32h7
 * @param led1 Pointer to led Gpio object 
 * @param led2 Pointer to led Gpio object 
 * @param led3 Pointer to led Gpio object 
 */
void init(Gpio* led1, Gpio* led2, Gpio* led3);

/**
 * @brief Outputs command usage and handles led toggle requests
 * @param args CmdArgs object with argument count and tokenized run arguments
 * @param txOut Output that we want to share in CLI, could be requested data or errors
 */
bool blink_cmd_handler(CmdArgs& args, std::span<uint8_t> txOut);

}  // namespace FlashCmd

}  // namespace LBR