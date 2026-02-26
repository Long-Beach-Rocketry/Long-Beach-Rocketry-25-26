#include <cstddef>
#include <functional>
#include <span>
#include "board.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    board_init();
    Board hw = get_board();

    while (1)
    {
#ifdef STM32H723xx
        std::span<std::reference_wrapper<Gpio>> leds{
            std::ref(hw.led1), std::ref(hw.led2), std::ref(hw.led3)};

        // Forward
        for (auto& led : leds)
        {
            led.get().set(1);
            for (volatile size_t i = 0; i < 100000; i++);
            led.get().set(0);
        }

        // Backward
        for (int i = leds.size() - 2; i > 0; i--)
        {
            leds[i].get().set(1);
            for (volatile size_t i = 0; i < 100000; i++);
            leds[i].get().set(0);
        }
#elif STM32L476xx
        hw.led1.toggle();
        for (volatile size_t i = 0; i < 100000; i++);
#endif
    }

    return 0;
}
