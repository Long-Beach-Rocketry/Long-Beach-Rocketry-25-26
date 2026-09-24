#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string_view>
#include "cmd.h"
#include "gpio.h"
#include "ring_buffer.h"

namespace LBR
{
constexpr uint8_t kMaxCommands{10};
constexpr uint8_t kMaxInputLen{64};

class Cli
{
public:
    bool register_cmd(Cmd command);
    bool help(/* Pass the data back through here in some format*/) const;
    bool process(uint8_t rx);

private:
    bool invoke(std::string_view input);
    void tokenize(CmdArgs& args);
    RingBuffer<uint8_t, kMaxInputLen> rxBuffer{};
    std::array<Cmd, kMaxCommands> commands;
    uint8_t count{0};
};

}  // namespace LBR