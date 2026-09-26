#pragma once
#include <cstdint>
#include <span>
#include <string_view>

namespace LBR
{
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