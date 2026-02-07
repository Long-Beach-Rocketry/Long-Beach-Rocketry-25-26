#include <cstdint>
#include "board.h"
#include "st_gpio.h"
#include "st_pwm.h"

static constexpr uint16_t MIN_PULSE_WIDTH = 500;
static constexpr uint16_t MAX_PULSE_WIDTH = 2500;
static constexpr uint16_t MAX_ANGLE = 180;

namespace LBR
{
// Output pin config for Timer 4, Channel 3 (PB8)
Stml4::StGpioSettings pwm_output_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 2};

const Stml4::StGpioParams pwm_output_params{pwm_output_settings, 8, GPIOB};

// PWM Config (TIM4 CH3)
Stml4::StPwmSettings pwm_settings{Stml4::PwmMode::EDGE_ALIGNED,
                                  Stml4::PwmOutputMode::MODE1,
                                  Stml4::PwmDir::UPCOUNTING};

const Stml4::StPwmParams pwm_params{TIM4, 3, pwm_settings};

// Create PWM GPIO and PWM objects
Stml4::HwGpio pwm_output(pwm_output_params);
Stml4::HwPwm pwm(pwm_params);

// Create Servo Motor object
const ServoMotorParams servo_motor_params{pwm, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH,
                                          MAX_ANGLE};
ServoMotor servo_motor(servo_motor_params);

Board board{.servo_motor = servo_motor};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;

    // Initialize PWM and pins
    bool ret = true;

    ret = ret && pwm_output.init();
    ret = ret && pwm.init();
    ret = ret && servo_motor.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR