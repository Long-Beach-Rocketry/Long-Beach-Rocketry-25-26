/**
 * @file cli.h
 * @brief The cli manager module
 * @author Joseph Chang
 */

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

/* 
TODO: I dont like current initialization structure - make CliSettings {promptname, *usart}; also promptName -> cliPrefix or promptPrefix
*/

class Cli
{
public:
    /**
     * @brief The Cli constructor
     * @param promptName_ The prefix like in other consoles,
     * for example `PS> ` or `$ `
     */
    Cli(std::string_view promptName_);

    /**
     * @brief Initialization function that stores a pointer to usart
     * @param usart_ A pointer to the usart which will be used to output text
     * 
     */
    void init(Usart* usart_);

    /**
     * @brief Simple getter for the CLI prompt prefix
     * @return Span consisting of the uint8_t characters in the prompt prefix
     */
    std::span<const uint8_t> get_prompt() const;

    /**
     * @brief Registers a command that can be called in the CLI later
     * 
     * @param command A user-defined command that will be registered
     *  and routed to if called
     * @return true if successful, false otherwise
     */
    bool register_cmd(Cmd command);

    /**
     * @brief CLI help function which lists available commands that have been registered
     * @return true if successful, false otherwise
     */
    bool list_commands() const;

    /**
     * @brief Tokenizes the inputs in the ring buffer and invokes the command if valid
     * @return true if successful, false otherwise
     */
    bool process();

    /**
     * @brief Adds the input buffer given an input from the user or pops if backspace
     * 
     * @param rx The user input
     * @return true if end of line was sent (so set update flag true and handle it)
     */
    bool take_input(uint8_t rx);

private:
    /**
     * @brief Searches for the command called, invokes if exists, and outputs result
     * 
     * @param input The command alias/name
     * @param cmdArgs Object with command count and run argument list
     * @return true if successful, false otherwise
     */
    bool invoke(std::string_view input, CmdArgs cmdArgs);

    /**
     * 
     */
    RingBuffer<uint8_t, kMaxInputLen> rxBuffer{};

    /**
     * @brief Array of registered commands in the CLI
     */
    std::array<Cmd, kMaxCommands> commands;

    /**
     * @brief Used to count number of registered commands to allow iterating through
     */
    uint8_t commandCount{0};

    Usart* usart{nullptr};
    bool init_flag{false};

    /**
     * @brief The cli prefix prompt like in other terminals or shells. Ex: `>`, `$`
     */
    std::string_view promptName{};
};

}  // namespace LBR