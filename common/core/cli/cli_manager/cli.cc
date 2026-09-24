#include "cli.h"
namespace LBR
{

bool Cli::help(/* Pass the data back through here in some format*/) const
{
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
        case '\n':
        case '\r':
            rxBuffer.push(rx);
            /* TODO create string_view */
            // return invoke()
            break;
        case '\177':
            rxBuffer.pop(rx);
            break;
        default:
            rxBuffer.push(rx);
    }
    return true;
}
bool Cli::invoke(std::string_view input)
{
    //  for (auto cmd : commands)
    //  {
    //      if (cmd.name == input)
    //      {
    //          CmdArgs args{.argc = 0, .argv = };
    //          cmd.invoke();
    //      }
    //  }
    return true;
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