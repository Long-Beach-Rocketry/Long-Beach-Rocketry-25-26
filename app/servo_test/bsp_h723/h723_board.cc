/**
 * @file h723_board.cc
 * @brief STM32H723 BSP for servo test app
 *
 * Pin assignments:
 *   PWM: TIM4 CH3 -> PB8 (AF2)  <- servo signal wire
 */

#include <cstdint>
#include "board.h"
#include "st_gpio.h"
#include "st_pwm.h"

// -----------------------------------------------------------------------
// Tune these after physical calibration
// -----------------------------------------------------------------------
static constexpr uint16_t MIN_PULSE_WIDTH = 500;   // microseconds at 0° (0.5ms)
static constexpr uint16_t MAX_PULSE_WIDTH = 2500;  // microseconds at MAX_ANGLE (2.5ms)
static constexpr uint16_t MAX_ANGLE       = 270;   // degrees (270° servo)

namespace LBR
{

uint32_t kPclkFreq = 64000000;  // confirm with Kent if clock speed differs

// Output pin config for Timer 4, Channel 3 (PB8, AF2)
Stmh7::StGpioSettings pwm_output_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::PUSH_PULL,
    Stmh7::GpioOspeed::LOW, Stmh7::GpioPupd::NO_PULL, 2};

const Stmh7::StGpioParams pwm_output_params{pwm_output_settings, 8, GPIOB};

// PWM config (TIM4 CH3)
Stmh7::StPwmSettings pwm_settings{Stmh7::PwmMode::EDGE_ALIGNED,
                                   Stmh7::PwmOutputMode::MODE1,
                                   Stmh7::PwmDir::UPCOUNTING};

const Stmh7::StPwmParams pwm_params{TIM4, Stmh7::PwmChannel::CH3,
                                     pwm_settings, kPclkFreq};

Stmh7::HwGpio pwm_output(pwm_output_params);
Stmh7::HwPwm  pwm(pwm_params);

// Servo motor
const ServoMotorParams servo_motor_params{pwm, MIN_PULSE_WIDTH,
                                          MAX_PULSE_WIDTH, MAX_ANGLE};
ServoMotor servo_motor(servo_motor_params);

Board board{.servo_motor = servo_motor};

bool bsp_init()
{
    RCC->AHB4ENR  |= RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_TIM4EN;

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
