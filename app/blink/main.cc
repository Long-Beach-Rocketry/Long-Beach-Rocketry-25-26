// TODO: Replace busy-waits with timer interrupts
#include <array>
#include <cstddef>
#include <functional>
#include "board.h"
#include "delay.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    board_init();
    Board& hw = get_board();

#ifdef STM32H723xx
    std::array<std::reference_wrapper<Gpio>, 3> leds{
        std::ref(hw.led1), std::ref(hw.led2), std::ref(hw.led3)};
    leds[0].get().set(1);

#endif

    while (1)
    {
#ifdef STM32H723xx
        // Forward
        for (size_t i = 0; i < leds.size() - 1; i++)
        {
            for (volatile size_t j = 0; j < 800000; j++);
            leds[i].get().set(0);
            leds[i + 1].get().set(1);
        }

        // Backward
        for (int i = leds.size() - 1; i > 0; i--)
        {
            for (volatile size_t j = 0; j < 800000; j++);
            leds[i].get().set(0);
            leds[i - 1].get().set(1);
        }
#endif
#ifdef STM32L476xx
        hw.led1.toggle();
        for (volatile size_t i = 0; i < 300000; i++);
#endif
    }

    return 0;
}
