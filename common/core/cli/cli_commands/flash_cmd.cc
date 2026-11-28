/**
 * @file flash_cmd.cc
 * @brief Implementation of the W25Q reset command
 */

#include "flash_cmd.h"
#include <algorithm>
#include <cstdio>
#include <span>
#include <string_view>
#include "string_span.h"
#include "w25q.h"

namespace LBR
{
namespace FlashCmd
{

static W25q* flash{nullptr};

void init(W25q* flash_)
{
    flash = flash_;
}

bool flash_cmd_handler(const CmdArgs& args, std::span<uint8_t> txOut)
{
    if (args.argc == 1)
    {
        const std::string_view operation{args.argv[0]};

        if (operation == "-h" || operation == "--help")
        {
            const std::span<const uint8_t> help = sv_to_span(flashCmd.desc);
            if (txOut.size() >= help.size())
            {
                std::copy(help.begin(), help.end(), txOut.begin());
            }

            return txOut.size() >= help.size();
        }
        //reset w25q flash chip
        if (operation == "reset")
        {
            if (flash == nullptr)  //if nullpointer
            {
                std::snprintf(reinterpret_cast<char*>(txOut.data()),
                              txOut.size(),
                              "ERROR: W25Q flash not found! \r\n\n");
                return false;
            }

            if (flash->reset())  //if  reset flash
            {
                std::snprintf(reinterpret_cast<char*>(txOut.data()),
                              txOut.size(),
                              "W25Q flash reset successful! \r\n\n");
                return true;
            }
            //else: reset did not go through
            std::snprintf(reinterpret_cast<char*>(txOut.data()), txOut.size(),
                          "ERROR: W25Q reset failed\r\n\n");
            return false;
        }
    }

    std::snprintf(reinterpret_cast<char*>(txOut.data()), txOut.size(),
                  "Usage: w25q reset\r\n"
                  "       w25q --help\r\n\n");
    return false;
}

const Cmd FlashCmd = {.name = "w25q",
                      .desc =
                          "Usage: w25q reset\r\n\n"
                          "Reset the W25Q flash chip.\r\n"
                          "Stored flash data is not erased.\r\n\n"
                          "Options:\r\n"
                          "\treset\t\tReset the W25Q device.\r\n"
                          "\t-h, --help\tShow this message.\r\n",
                      .invoke = flash_cmd_handler,
                      .repeatable = false};

}  // namespace FlashCmd
}  // namespace LBR