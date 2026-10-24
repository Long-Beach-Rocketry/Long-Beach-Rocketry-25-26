/**
 * @file cli.h
 * @brief The cli manager module
 * @author Joseph Chang
 */

#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <span>
#include <string_view>
#include "cmd.h"
#include "delay.h"
#include "ring_buffer.h"
#include "string_span.h"
#include "usart.h"

namespace LBR
{
constexpr uint8_t kMaxCommands{10};
constexpr uint8_t kMaxArgs{8};
constexpr uint8_t kMaxArgLen{(kMaxInputLen - kMaxArgs) / kMaxArgs};

struct CliSettings
{
    std::string_view promptName;
    Usart* usart;
};

class Cli
{
public:
    /**
     * @brief The Cli constructor method
     * @param settings Cli settings structure with uart pointern prompt prefix
     * for example `PS> ` or `$ `
     */
    Cli(CliSettings settings);

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
     * @brief Checks whether a command should be run by looking at the flags and executes if so
     * @return true if a command was invoked and invoked successfully, false otherwise
     */
    bool run();

    /**
     * @brief Adds the input buffer given an input from the user or pops if backspace
     * 
     * @param rx The user input
     * @return true if end of line was sent (so set update flag true and handle it)
     */
    bool take_input(uint8_t rx);

private:
    /************************ Member Methods ************************/
    /**
     * @brief Tokenizes the inputs in the ring buffer and invokes the command if valid
     * @return true if successful, false otherwise
     */
    bool process();

    /**
     * @brief Searches for the command called, invokes if exists, and outputs result
     * 
     * @param input The command alias/name
     * @param cmdArgs Object with command count and run argument list
     * @return true if successful, false otherwise
     */
    bool invoke(std::string_view input, CmdArgs& cmdArgs);

    /**
     * @brief Attempts to invoke the most recently called command
     * @return true if successful, false otherwise
     */
    bool repeat();

    /**
     * @brief Echos the input back to the serial terminal

     * @param rx The character input from the user
     * @return true if the usart sends successfully, false otherwise
     */
    bool echo_input(uint8_t rx);

    /**
     * @brief Saves the command arguments in case of repeat called
     *
     * @param args The commands args we want to deep copy
     * @return true if successful, false otherwise
     */
    bool save(const CmdArgs& args);

    /************************ Member Variables ************************/

    /**
     * @brief Array of registered commands in the CLI
     */
    std::array<Cmd, kMaxCommands> commands;

    /**
     * @brief Used to count number of registered commands to allow iterating through
     */
    uint8_t commandCount{0};

    RingBuffer<uint8_t, kMaxInputLen> rxBuffer{};

    Usart* usart{nullptr};

    /**
     * @brief The cli prefix prompt like in other terminals or shells. Ex: `>`, `$`
     */
    std::string_view _promptName{};

    /**
     * @brief Flag for if a command was invoked
     */
    bool invoke_flag{false};

    /**
     * @brief Flag for if a command was invoked to run repeatedly
     */
    bool repeat_flag{false};
    std::string_view last_cmd_alias{""};
    CmdArgs last_cmd_args{.argc = 0, .argv = nullptr};
    char argStorage[kMaxArgs][kMaxArgLen]{};
    const char* m_argPtrs[kMaxArgs]{};
};

}  // namespace LBR