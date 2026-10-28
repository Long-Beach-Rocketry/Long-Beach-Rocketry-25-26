#include "board.h"
#include "logger.h"

using namespace LBR;

int main(int argc, char** argv)
{
    bsp_init();

    Board& hw = get_board();
    Logger logger(hw.usart);

    while (1)
    {
        logger.log_debug("read");
        logger.log_error("if");
        logger.log_warn("gay");
        logger.log_info("XD");

        for (volatile uint32_t i = 0; i < 1000000; i++)
        {
        }
    }

    return 0;
}
