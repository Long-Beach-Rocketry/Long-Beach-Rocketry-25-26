/**
 * @file repeat_cmd.h
 * @brief This is meant as a stub for the repeat command to help in outputting description
 * @author Joseph Chang
 */

#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <span>
#include "cmd.h"
#include "string_span.h"

namespace LBR
{

namespace RepeatCmd
{
extern const Cmd repeatCmd;
extern uint16_t msDelay;

/**
 * @brief Outputs command usage
 * @param args CmdArgs object with argument count and tokenized run arguments
 * @param txOut Output that we want to share in CLI, could be requested data or errors
 * @return true if attempt to repeat, false otherwise
 */
bool repeat_cmd_handler(CmdArgs& args, std::span<uint8_t> txOut);

}  // namespace RepeatCmd

}  // namespace LBR