#include <cstdint>
#include "board.h"
#include "drv8245.h"
#include "st_gpio.h"
#include "st_pwm.h"

namespace LBR
{

uint32_t kPclkFreq = 64000000;

/// CONFIG: PWM output pin (PB8, TIM4 CH3)

Stmh7::StGpioSettings pwm_output_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::PUSH_PULL,
    Stmh7::GpioOspeed::LOW, Stmh7::GpioPupd::NO_PULL, 2};

const Stmh7::StGpioParams pwm_output_params{pwm_output_settings, 8, GPIOB};

/// CONFIG: DIR pin (PA0)

Stmh7::StGpioSettings dir_settings{
    Stmh7::GpioMode::GPOUT, Stmh7::GpioOtype::PUSH_PULL, Stmh7::GpioOspeed::LOW,
    Stmh7::GpioPupd::NO_PULL, 0};

const Stmh7::StGpioParams dir_params{dir_settings, 0, GPIOA};

/// CONFIG: PWM (TIM4 CH3)

Stmh7::StPwmSettings pwm_settings{Stmh7::PwmMode::EDGE_ALIGNED,
                                  Stmh7::PwmOutputMode::MODE1,
                                  Stmh7::PwmDir::UPCOUNTING};

const Stmh7::StPwmParams pwm_params{TIM4, Stmh7::PwmChannel::CH3, pwm_settings,
                                    kPclkFreq};

/// CREATE: PWM GPIO, PWM, and DRV8245 objects
Stmh7::HwGpio pwm_output(pwm_output_params);
Stmh7::HwPwm pwm(pwm_params);
Stmh7::HwGpio dir(dir_params);

Board board{.pwm = pwm, .dir = dir};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_TIM4EN;

    // Initialize PWM and pins
    bool ret = true;

    ret = ret && pwm_output.init();
    ret = ret && pwm.init();
    ret = ret && dir.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR