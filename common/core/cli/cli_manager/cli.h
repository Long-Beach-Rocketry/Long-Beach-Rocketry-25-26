#pragma once

#include <array>
#include <cstdint>
#include "cmd.h"

namespace LBR
{
constexpr uint8_t kMaxCommands{10};

class Cli
{
public:
private:
    std::array<Cmd, kMaxCommands> commands;
};

}  // namespace LBR