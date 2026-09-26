#include "blink_cmd.h"
#include <algorithm>
#include <array>
#include <cstring>
#include "gpio.h"
#include "string_span.h"

namespace LBR
{
static std::array<Gpio*, 3> gpios{nullptr, nullptr, nullptr};

namespace BlinkCmd
{

void init(Gpio* led1, Gpio* led2, Gpio* led3)
{
    gpios[0] = led1;
    gpios[1] = led2;
    gpios[2] = led3;
}

bool blink_cmd_handler(const CmdArgs& args, std::span<uint8_t> txOut)
{
    std::span<const uint8_t> error = sv_to_span(kCmdErrorArgs);
    std::copy(error.begin(), error.end(),
              txOut.subspan(0, error.size()).begin());

    std::span<const uint8_t> success = sv_to_span(kCmdSuccess);
    bool ret{false};

    if (args.argc > 0)
    {
        std::string_view op1(args.argv[0]);
        if (op1 == "--help" || op1 == "-h")
        {
            std::span<const uint8_t> desc = sv_to_span(blinkCmd.desc);
            std::copy(desc.begin(), desc.end(),
                      txOut.subspan(0, desc.size()).begin());
            return true;
        }
        else if ((op1 == "-o" || op1 == "--option"))
        {
            if (args.argc > 2)
            {
                return false;
            }
            std::string_view op2(args.argv[1]);
            if (op2 == "1" && gpios[0])
            {
                ret = gpios[0]->toggle();
                if (ret)
                {
                    std::copy(success.begin(), success.end(),
                              txOut.subspan(0, success.size()).begin());
                }
                return ret;
            }
            else if (op2 == "2" && gpios[1])
            {
                ret = gpios[1]->toggle();
                if (ret)
                {
                    std::copy(success.begin(), success.end(),
                              txOut.subspan(0, success.size()).begin());
                }
            }
            else if (op2 == "3" && gpios[2])
            {
                ret = gpios[2]->toggle();
                if (ret)
                {
                    std::copy(success.begin(), success.end(),
                              txOut.subspan(0, success.size()).begin());
                }
            }
        }
        return ret;
    }
    else
    {

        for (auto gpio : gpios)
        {
            if (gpio)
            {
                ret |= gpio->toggle();
            }
        }
        if (ret)
        {
            std::copy(success.begin(), success.end(),
                      txOut.subspan(0, success.size()).begin());
        }
        return ret;
    }
    return false;
}

const Cmd blinkCmd = {
    .name = "blink",
    .desc =
        "Usage: blink [LED PIN]\r\n\nToggles the pin of an "
        "led.\r\n\nOptions:\r\n\t-o, --option [1|2|3]\tToggles the selected "
        "pin\r\n\n\t-h, --help\t\tShows this message.\r\n",
    .invoke = blink_cmd_handler};

}  // namespace BlinkCmd

}  // namespace LBR