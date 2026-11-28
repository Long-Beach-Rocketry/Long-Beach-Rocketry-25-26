/**
 * @file w25q_cmd.h
 * @author Aaron Oh
 * @brief CLI command for the W25Q flash chip
 */

#pragma once

#include <span>
#include "cmd.h"

namespace LBR
{

class W25q;

namespace FlashCmd
{

extern const Cmd FlashCmd;

/**
 * @brief Give the W25Q command its flash driver object
 */
void init(W25q* flash);

/**
 * @brief Handle W25Q CLI arguments and write the result to txOut
 */
bool flash_cmd_handler(const CmdArgs& args, std::span<uint8_t> txOut);

}  // namespace FlashCmd
}  // namespace LBR