#pragma once
#include <cstdint>
#include <span>
#include <string_view>

namespace LBR
{
constexpr uint8_t kMaxInputLen{255};
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
    std::string_view name;
    std::string_view desc;
    bool (*invoke)(const CmdArgs& args, std::span<uint8_t> txOut);
};

}  // namespace LBR