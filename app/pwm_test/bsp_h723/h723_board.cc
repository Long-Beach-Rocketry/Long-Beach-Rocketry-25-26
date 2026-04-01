#include <cstdint>
#include "board.h"
#include "st_gpio.h"
#include "st_pwm.h"

namespace LBR
{
// Output pin config for Timer 4, Channel 3 (PB8)
Stmh7::StGpioSettings pwm_output_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::PUSH_PULL,
    Stmh7::GpioOspeed::LOW, Stmh7::GpioPupd::NO_PULL, 2};

const Stmh7::StGpioParams pwm_output_params{pwm_output_settings, 8, GPIOB};

// PWM Config (TIM4 CH3)
Stmh7::StPwmSettings pwm_settings{Stmh7::PwmMode::EDGE_ALIGNED,
                                  Stmh7::PwmOutputMode::MODE1,
                                  Stmh7::PwmDir::UPCOUNTING};

const Stmh7::StPwmParams pwm_params{TIM4, Stmh7::PwmChannel::CH3, pwm_settings};

// Create PWM GPIO and PWM objects
Stmh7::HwGpio pwm_output(pwm_output_params);
Stmh7::HwPwm pwm(pwm_params);

Board board{.pwm = pwm};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_TIM4EN;

    // Initialize PWM and pins
    bool ret = true;

    ret = ret && pwm_output.init();
    ret = ret && pwm.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR