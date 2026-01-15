#include <cstdint>
#include "drv8245.h"
#include "board.h"
#include "st_gpio.h"
#include "st_pwm.h"

namespace LBR {

// GPIO configs 
Stml4::StGpioSettings dir_settings{
    Stml4::GpioMode::GPOUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::MEDIUM, Stml4::GpioPupd::NO_PULL, 0
};
const Stml4::StGpioParams dir_params{dir_settings, 9, GPIOA}; // PA9

Stml4::StGpioSettings slp_settings{
    Stml4::GpioMode::GPOUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::MEDIUM, Stml4::GpioPupd::NO_PULL, 0
};
const Stml4::StGpioParams slp_params{slp_settings, 7, GPIOC}; // PC7

Stml4::StGpioSettings drv_z_settings{
    Stml4::GpioMode::GPOUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::MEDIUM, Stml4::GpioPupd::NO_PULL, 0
};
const Stml4::StGpioParams drv_z_params{drv_z_settings, 8, GPIOC}; // PC8

Stml4::StGpioSettings fault_settings{
    Stml4::GpioMode::INPUT, Stml4::GpioOtype::PUSH_PULL, 
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::PULL_UP, 0
};
const Stml4::StGpioParams fault_params{fault_settings, 6, GPIOC}; // PC6

// Output pin config for Timer 4, Channel 3 (PB8)
Stml4::StGpioSettings pwm_output_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 2};

const Stml4::StGpioParams pwm_output_params{pwm_output_settings, 8, GPIOB}; // PB8

// PWM configs
// PWM (TIM4 CH3)
Stml4::StPwmSettings pwm_settings{Stml4::PwmMode::EDGE_ALIGNED,
                                  Stml4::PwmOutputMode::MODE1,
                                  Stml4::PwmDir::UPCOUNTING};

const Stml4::StPwmParams pwm_params{TIM4, 3, pwm_settings}; // TIM4 CH3

// GPIO and PWM objects
Stml4::HwGpio dir_(dir_params);
Stml4::HwGpio slp_(slp_params);
Stml4::HwGpio drv_z_(drv_z_params);
Stml4::HwGpio fault_(fault_params);

Stml4::HwGpio pwm_output(pwm_output_params);
Stml4::HwPwm pwm(pwm_params);

Drv8245 motor(dir_, pwm, drv_z_, slp_, fault_);
Board board{motor};

bool bsp_init()
{
    // Enable peripheral clocks for GPIOA, GPIOC, and TIM4
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;

    // Initialize all pins and PWM
    bool ret = true;
    ret = ret && dir_.init();
    ret = ret && slp_.init();
    ret = ret && drv_z_.init();
    ret = ret && fault_.init();
    ret = ret && pwm.init();

    // Set defaults
    drv_z_.set(true);   // Coast (outputs disabled)
    slp_.set(false);     // Sleep (optional: keep asleep until ready)
    dir_.set(false);     // Default direction
    // pwm.setDutyCycle(0); // No drive

    // Optionally, wake up and enable when ready
    slp_.set(true);    // Wake up
    drv_z_.set(false); // Enable outputs

    motor.init();

    return ret;
}

Board& get_board()
{
    static Board board{.motor = motor};
    return board;
}

} // namespace LBR