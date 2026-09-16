#include "board.h"
#include "st_gpio.h"

namespace LBR
{
namespace Stmh7
{
StGpioSettings ld_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                           GpioOspeed::LOW, GpioPupd::NO_PULL, 0};
StGpioParams ld1_params{ld_settings, 0, GPIOB};
StGpioParams ld2_params{ld_settings, 1, GPIOE};
StGpioParams ld3_params{ld_settings, 14, GPIOB};

HwGpio ld1{ld1_params};
HwGpio ld2{ld2_params};
HwGpio ld3{ld3_params};
}  // namespace Stmh7

Board board{.led1 = Stmh7::ld1, .led2 = Stmh7::ld2, .led3 = Stmh7::ld3};

bool board_init()
{
    bool result = false;

    // Enable GPIOB and GPIOE clock
    RCC->AHB4ENR |= (RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIOEEN);

    result = Stmh7::ld1.init();
    result = Stmh7::ld2.init();
    result = Stmh7::ld3.init();

    return result;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR
