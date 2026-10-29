#include "board.h"

using namespace LBR;

int main(int argc, char** argv)
{
    bsp_init();

    Board& hw = get_board();
    Logger logger(hw.usart);

    while (1)
    {
        logger.log_debug("debug");
        logger.log_error("error");
        logger.log_warn("warn");
        logger.log_info("info");

        for (volatile uint32_t i = 0; i < 1000000; i++)
        {
        }
    }

    return 0;
}
