/**
 * @file cmd.h
 * @brief Module with base structure for user-definable commands
 * @author Joseph Chang
 */

#pragma once
#include <cstdint>
#include <span>
#include <string_view>

namespace LBR
{
constexpr uint16_t kMaxInputLen{320};
constexpr std::string_view kCmdErrorNotFound = "ERROR: Command not found\r\n\n";
constexpr std::string_view kCmdErrorArgs = "ERROR: Invalid arguments\r\n\n";
constexpr std::string_view kCmdSuccess =
    "SUCCESS: Command ran successfully\r\n\n";

struct CmdArgs
{
    uint8_t argc;
    const char** argv;
};

struct Cmd
{
    /**
     * @brief Command alias that will be used to call a command
     */
    std::string_view name;

    /**
     * @brief Description of the command
     */
    std::string_view desc;

    /**
     * @brief Function pointer that is called when the command is called. 
     * Must return bool and take CmdArgs for arguments and txOut to output
     * text to usart
     */
    bool (*invoke)(const CmdArgs& args, std::span<uint8_t> txOut);
};

}  // namespace LBR