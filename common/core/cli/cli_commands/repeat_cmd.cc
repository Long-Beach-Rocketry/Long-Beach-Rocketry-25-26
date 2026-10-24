/**
 * @file repeat_cmd.cc
 * @brief Implementation of repeat command methods and the repeat command
 */

#include "repeat_cmd.h"

namespace LBR
{

namespace RepeatCmd
{

uint16_t msDelay{1000};
static constexpr uint16_t kMinRepeatDelay{10};

constexpr std::string_view kRepeatCmdDefaultOut =
    "INFO: Attempting to repeat "
    "- if fails, the most recent command may not be repeatable\r\n\n";

bool repeat_cmd_handler(const CmdArgs& args, std::span<uint8_t> txOut)
{

    std::span<const uint8_t> info = sv_to_span(kRepeatCmdDefaultOut);
    std::copy(info.begin(), info.end(), txOut.subspan(0, info.size()).begin());

    if (args.argc > 0)
    {
        std::string_view op1(args.argv[0]);
        if (op1 == "--help" || op1 == "-h")
        {
            std::span<const uint8_t> desc = sv_to_span(repeatCmd.desc);
            std::copy(desc.begin(), desc.end(),
                      txOut.subspan(0, desc.size()).begin());
            return false;
        }
        // See if its a delay arg instead
        uint16_t tmp{};
        const char* first = op1.data();
        const char* last = first + op1.size();
        auto result = std::from_chars(first, last, tmp);
        std::span<const uint8_t> error = sv_to_span(kCmdErrorArgs);

        if (result.ec != std::errc{} || result.ptr != last)
        {
            // Not good
        }
        else if (tmp >= kMinRepeatDelay)
        {
            msDelay = tmp;
            return true;
        }
        std::copy(error.begin(), error.end(),
                  txOut.subspan(0, error.size()).begin());
        return false;
    }

    return true;
}

const Cmd repeatCmd = {
    .name = "repeat",
    .desc =
        "Usage: repeat [options]\r\n\nInvokes the most "
        "recently executed command repeatedly.\r\n\nOptions:\r\n\t"
        "[delay]\tThe delay at which the command is repeated."
        "Default is 1000 ms; minimum is 10 ms.\r\n\n\t-h, --help\t\tShows this "
        "message.\r\n",
    .invoke = repeat_cmd_handler,
    .repeatable = false};

}  // namespace RepeatCmd

}  // namespace LBR