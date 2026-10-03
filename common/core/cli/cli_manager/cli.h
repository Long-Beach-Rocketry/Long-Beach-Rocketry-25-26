#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string_view>
#include "cmd.h"
#include "ring_buffer.h"
#include "string_span.h"
#include "usart.h"

namespace LBR
{
constexpr uint8_t kMaxCommands{10};
constexpr uint8_t kMaxArgs{8};

class Cli
{
public:
    Cli(std::string_view promptName_);
    void init(Usart* usart_);
    std::span<const uint8_t> get_prompt() const;
    bool register_cmd(Cmd command);
    bool list_commands() const;
    bool process();
    bool take_input(uint8_t rx);

private:
    bool invoke(std::string_view input, CmdArgs cmdArgs);
    RingBuffer<uint8_t, kMaxInputLen> rxBuffer{};
    std::array<Cmd, kMaxCommands> commands;
    uint8_t commandCount{0};
    Usart* usart{nullptr};
    bool init_flag{false};
    std::string_view promptName{};
};

}  // namespace LBR