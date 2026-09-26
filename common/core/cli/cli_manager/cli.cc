#include "cli.h"

namespace LBR
{
void Cli::init(Usart* usart_)
{
    usart = usart_;
    init_flag = true;
}

bool Cli::help(/* Pass the data back through here in some format*/) const
{
    if (!init_flag)
    {
        return false;
    }
    std::string_view spacer{"\t"};
    std::string_view newLine{"\r\n"};
    std::span<const uint8_t> txSpacer = sv_to_span(spacer);
    std::span<const uint8_t> txNewLine = sv_to_span(newLine);

    for (auto cmd : commands)
    {
        // Probably better way to do this by accumulating into one
        std::span<const uint8_t> txName = sv_to_span(cmd.name);
        std::span<const uint8_t> txDesc = sv_to_span(cmd.desc);
        usart->send(txName);
        usart->send(txSpacer);
        usart->send(txDesc);
        usart->send(txNewLine);
    }
    return true;
}

bool Cli::register_cmd(Cmd command)
{
    if (count >= kMaxCommands)
    {
        return false;
    }
    commands[count++] = command;
    return true;
}

bool Cli::process(uint8_t rx)
{
    switch (rx)
    {
        case '\177':
            rxBuffer.pop(rx);
            break;
        default:
            rxBuffer.push(rx);
    }
    if (rx != '\r' && rx != '\n')
    {
        return true;
    }

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

    if (argc > 0)
    {
        std::string_view input(argv[0]);
        CmdArgs cmdArgs{.argc = --argc,
                        .argv = (argc > 0) ? &argv[1] : nullptr};
        rxBuffer.reset();
        return invoke(input, cmdArgs);
    }

    return false;
}

bool Cli::invoke(std::string_view input, CmdArgs cmdArgs)
{
    for (auto cmd : commands)
    {
        if (cmd.name == input)
        {
            return cmd.invoke(cmdArgs);
        }
    }
    return false;
}

void Cli::tokenize(CmdArgs& args)
{
    return;
    //  uint8_t data{};
    //  uint8_t argc{0};
    //  uint8_t tokenIdx{0};
    //  while (!rxBuffer.empty())
    //  {
    //      rxBuffer.pop(data);
    //      if (data == ' ' && tokenIdx == 0)
    //      {
    //          continue;
    //      }
    //      else if (data == ' ')
    //      {
    //          ++argc;
    //          tokenIdx = 0;
    //      }
    //      else
    //      {
    //          args.argv[argc][tokenIdx] = data;
    //      }
    //  }
}

}  // namespace LBR