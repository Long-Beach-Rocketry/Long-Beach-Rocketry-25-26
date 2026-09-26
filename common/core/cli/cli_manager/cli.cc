#include "cli.h"

namespace LBR
{
Cli::Cli(std::string_view promptName_) : promptName(promptName_)
{
}

void Cli::init(Usart* usart_)
{
    usart = usart_;
    init_flag = true;
}

std::span<const uint8_t> Cli::get_prompt() const
{
    return sv_to_span(promptName);
}

bool Cli::list_commands() const
{
    if (!init_flag)
    {
        return false;
    }
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
    commands[commandCount++] = command;
    return true;
}

bool Cli::process()
{
    uint8_t rx{};
    uint8_t argc{0};
    static char tmp[kMaxInputLen]{};
    size_t idx{0};

    const char* argv[kMaxArgs] = {nullptr};
    bool inToken{false};

    while (rxBuffer.pop(rx))
    {
        if (rx == '\r' || rx == '\n')
        {
            if (idx > 0 || argc > 0)
            {
                tmp[idx] = '\0';
                break;
            }
            continue;
        }
        if (rx == ' ' || rx == '\t')
        {
            if (inToken)
            {
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
            tmp[idx++] = static_cast<char>(rx);
        }
    }

    rxBuffer.reset();

    if (argc > 0)
    {
        std::string_view input(argv[0]);
        CmdArgs cmdArgs{.argc = --argc,
                        .argv = (argc > 0) ? &argv[1] : nullptr};
        return invoke(input, cmdArgs);
    }
    return false;
}

bool Cli::invoke(std::string_view input, CmdArgs cmdArgs)
{
    bool ret{false};
    if (input == "list")
    {
        list_commands();
    }

    for (auto cmd : commands)
    {
        if (cmd.name == input)
        {
            std::array<uint8_t, kMaxInputLen> txOut{};
            ret = cmd.invoke(cmdArgs, txOut);
            if (!txOut.empty())
            {
                usart->send(txOut);
            }
            return ret;
        }
    }
    return false;
}

bool Cli::take_input(uint8_t rx)
{
    bool ret{false};
    switch (rx)
    {
        case '\177':  // backspace
            rxBuffer.pop(rx);
            return ret;
        case '\r':
        case '\n':
            ret = true;
        default:
            return rxBuffer.push(rx) && ret;
    }
    return false;
}

}  // namespace LBR