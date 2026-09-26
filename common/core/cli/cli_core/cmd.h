#pragma once
#include <cstdint>
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
    bool (*invoke)(const CmdArgs& args);
};

}  // namespace LBR