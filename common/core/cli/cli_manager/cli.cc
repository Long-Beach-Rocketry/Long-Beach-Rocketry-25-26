/**
 * @file cli.cc
 * @brief Implentation of cli class and methods
 */

#include "cli.h"

namespace LBR
{

Cli::Cli(CliSettings settings)
    : usart(settings.usart), _promptName(settings.promptName)
{
}

std::span<const uint8_t> Cli::get_prompt() const
{
    return sv_to_span(_promptName);
}

bool Cli::list_commands() const
{
    if (usart == nullptr)
    {
        return false;
    }
    // Output this helper title and then list of command aliases
    std::string_view title{
        "Registered Commands can be seen with the 'list' command.\r\n See more "
        "info about commands with <command> -h\r\n\n"};
    std::string_view newLine{"\r\n"};
    std::span<const uint8_t> txTitle = sv_to_span(title);
    std::span<const uint8_t> txNewLine = sv_to_span(newLine);

    usart->send(txTitle);

    for (int i = 0; i < commandCount; i++)
    {
        // Probably better way to do this by accumulating into one
        std::span<const uint8_t> txName = sv_to_span(commands[i].name);
        usart->send(txName);
        usart->send(txNewLine);
    }
    return true;
}

bool Cli::register_cmd(Cmd command)
{
    if (commandCount >= kMaxCommands)
    {
        return false;
    }
    else if (command.name == "list" || command.name == "q")
    {
        // reserved commands
        return false;
    }
    commands[commandCount++] = command;
    return true;
}

bool Cli::process()
{
    uint8_t rx{};
    uint8_t argc{0};
    char tmp[kMaxInputLen]{};
    size_t idx{0};

    const char* argv[kMaxArgs] = {nullptr};
    bool inToken{false};

    // Tokenize from ring buffer characters
    while (rxBuffer.pop(rx))
    {
        // End of command
        if (rx == '\r' || rx == '\n')
        {
            if (idx > 0 || argc > 0)
            {
                // Safe end of string
                tmp[idx] = '\0';
                break;
            }
            continue;
        }
        // Handle whitespace
        if (rx == ' ' || rx == '\t')
        {
            if (inToken)
            {
                // Safely terminate token
                tmp[idx++] = '\0';
                inToken = false;
            }
        }
        else
        {
            if (idx >= kMaxInputLen - 1)
            {
                break;
            }
            else if (!inToken)
            {
                if (argc < kMaxArgs)
                {
                    argv[argc++] = &tmp[idx];
                }
                inToken = true;
            }
            // Append char to current token
            tmp[idx++] = static_cast<char>(rx);
        }
    }

    rxBuffer.reset();

    // Way it was tokenized is alias is first token so handle that
    if (argc > 0)
    {
        std::string_view input(argv[0]);
        CmdArgs cmdArgs{.argc = --argc,
                        .argv = (argc > 0) ? &argv[1] : nullptr};
        return invoke(input, cmdArgs);
    }
    return false;
}

bool Cli::invoke(std::string_view input, CmdArgs& cmdArgs)
{
    bool ret{false};
    if (input == "list")
    {
        return list_commands();
    }
    else if (input == "repeat")
    {
        repeat_flag = true;
        return repeat();
    }

    for (auto cmd : commands)
    {
        if (cmd.name == input)
        {
            std::array<uint8_t, kMaxInputLen> txOut{};
            ret = cmd.invoke(cmdArgs, txOut);

            if (!txOut.empty())
            {
                ret &= usart->send(txOut);
            }
            if (ret)
            {
                last_cmd_alias = cmd.name;
                ret &= save(cmdArgs);
            }
            return ret;
        }
    }
    return false;
}

bool Cli::take_input(uint8_t rx)
{
    invoke_flag = echo_input(rx);
    return invoke_flag;
}

bool Cli::echo_input(uint8_t rx)
{
    if (usart == nullptr)
    {
        return false;
    }
    bool ret{false};
    switch (rx)
    {
        case '\177':  // backspace
        case '\b':    // other backspace, one of these is putty, other minicom
            if (rxBuffer.pop(rx))
            {
                /* Apparently, '\b' is backspace 
                 so its back (left), replace with space (right), back (left)
                */
                uint8_t erase_seq[3] = {'\b', ' ', '\b'};
                std::span<const uint8_t> tx_span(erase_seq, 3);
                usart->send(tx_span);
            }
            return ret;
        case '\r':
        case '\n':
            ret = true;
        default:
            if (repeat_flag)
            {
                repeat_flag = false;
                return false;
            }

            // Add input to ring buffer. NOTE: write policy is no overwrite
            std::span<const uint8_t> tx_span(&rx, 1);
            usart->send(tx_span);
            /* 
                IMPORTANT: Order here matters, if ret is false, compiler will just skip rxBuffer.push() so move it in front
            */
            return rxBuffer.push(rx) && ret;
    }
    return false;
}

bool Cli::repeat()
{
    if (usart == nullptr)
    {
        return false;
    }
    LBR::Utils::DelayMs(10);
    {
    }
    if (last_cmd_alias != "")
    {
        for (auto cmd : commands)
        {
            if (cmd.name == last_cmd_alias && cmd.repeatable)
            {
                bool ret{true};
                std::array<uint8_t, kMaxInputLen> txOut{};
                ret = cmd.invoke(last_cmd_args, txOut);

                if (!txOut.empty())
                {
                    ret &= usart->send(txOut);
                }
                return ret;
            }
        }
    }
    repeat_flag = false;
    return false;
}

bool Cli::run()
{
    if (invoke_flag)
    {
        invoke_flag = false;
        process();
    }
    else if (repeat_flag)
    {
        repeat();
    }
    else
    {
        return false;
    }
    return true;
}

bool Cli::save(const CmdArgs& args)
{
    for (int i = 0; i < args.argc; i++)
    {
        strncpy(argStorage[i], args.argv[i], kMaxArgLen - 1);
        argStorage[i][kMaxArgLen - 1] = '\0';
        argPtrs[i] = argStorage[i];
    }
    last_cmd_args.argc = args.argc;
    last_cmd_args.argv = argPtrs;
    return true;
}

}  // namespace LBR