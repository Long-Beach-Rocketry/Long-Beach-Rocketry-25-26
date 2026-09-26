#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string_view>
#include "cmd.h"
#include "gpio.h"
#include "ring_buffer.h"
#include "string_span.h"
#include "usart.h"

namespace LBR
{
constexpr uint8_t kMaxCommands{10};
constexpr uint8_t kMaxArgs{8};
constexpr uint8_t kMaxInputLen{255};

class Cli
{
public:
    void init(Usart* usart_);
    bool register_cmd(Cmd command);
    bool help() const;
    bool process();
    bool take_input(uint8_t rx);

private:
    bool invoke(std::string_view input, CmdArgs cmdArgs);
    RingBuffer<uint8_t, kMaxInputLen> rxBuffer{};
    std::array<Cmd, kMaxCommands> commands;
    uint8_t count{0};
    Usart* usart{nullptr};
    bool init_flag{false};
};

}  // namespace LBR