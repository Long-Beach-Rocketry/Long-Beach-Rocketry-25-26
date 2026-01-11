#include <cstdint>
#include "board.h"
//#include "st_uart.h"
//#include "st_adc.h"
//#include "st_pwm.h"
//#include st_encoder driver when written
#include "st_gpio.h"

namespace LBR 
{

// UART4_TX (PA0) & UART4_RX (PA1) pin config
Stml4::StGpioSettings tx_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams tx_params{tx_settings, 0, GPIOA};

Stml4::StGpioSettings rx_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams rx_params{rx_settings, 1, GPIOA};

// LMT_SWT (PA3) pin config 
Stml4::StGpioSettings lmt_swt_settings{
    Stml4::GpioMode::INPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::PULL_UP, 0};
const Stml4::StGpioParams lmt_swt_params{lmt_swt_settings, 3, GPIOA};

// Motor Driver Control Pins
// RS_EN (PA5) pin config
Stml4::StGpioSettings rs_en_settings{
    Stml4::GpioMode::OUTPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams rs_en_params{rs_en_settings, 5, GPIOA};
// MTR_PWM1 (PA8) pin config
Stml4::StGpioSettings mtr_pwm1_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams mtr_pwm1_params{mtr_pwm1_settings, 8, GPIOA};
// MTR_DIR2 (PA9) pin config
Stml4::StGpioSettings mtr_dir2_settings{
    Stml4::GpioMode::OUTPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams mtr_dir2_params{mtr_dir2_settings, 9, GPIOA};

// SWDIO (PA13) & SWCLK (PA14) pin config
// (no need to configure these unless SWD is disabled)
Stml4::StGpioSettings swdio_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams swdio_params{swdio_settings, 13, GPIOA};

Stml4::StGpioSettings swclk_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams swclk_params{swclk_settings, 14, GPIOA};

// DBG_USART_TX (PB6) & DBG_USART_RX (PB7) pin config
Stml4::StGpioSettings dbg_tx_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams dbg_tx_params{dbg_tx_settings, 6, GPIOB};

Stml4::StGpioSettings dbg_rx_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams dbg_rx_params{dbg_rx_settings, 7, GPIOB};

// ADC Pins
// CS_PADC (PC0) 
Stml4::StGpioSettings cs_padc_settings{
    Stml4::GpioMode::OUTPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams cs_padc_params{cs_padc_settings, 0, GPIOC};

// Encoder Pins
// ENC_A_L (PC4) pin config
Stml4::StGpioSettings enc_a_l_settings{
    Stml4::GpioMode::INPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams enc_a_l_params{enc_a_l_settings, 4, GPIOC};
// ENC_B_L (PC5) pin config
Stml4::StGpioSettings enc_b_l_settings{
    Stml4::GpioMode::INPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams enc_b_l_params{enc_b_l_settings, 5, GPIOC};

// DR_FAULT (PC6) / MTR_SLP (PC7) / DRV_Z (PC8) 
Stml4::StGpioSettings drv_fault_settings{
    Stml4::GpioMode::INPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams drv_fault_params{drv_fault_settings, 6, GPIOC};

Stml4::StGpioSettings mtr_slp_settings{
    Stml4::GpioMode::OUTPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams mtr_slp_params{mtr_slp_settings, 7, GPIOC};

Stml4::StGpioSettings drv_z_settings{
    Stml4::GpioMode::INPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams drv_z_params{drv_z_settings, 8, GPIOC};

// Debug LEDs 
// DBG_RED (PC1) pin config
Stml4::StGpioSettings dbg_red_settings{
    Stml4::GpioMode::OUTPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams dbg_red_params{dbg_red_settings, 1, GPIOC};
// DBG_BLUE (PC2) pin config
Stml4::StGpioSettings dbg_blue_settings{
    Stml4::GpioMode::OUTPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};   
const Stml4::StGpioParams dbg_blue_params{dbg_blue_settings, 2, GPIOC};
// DBG_GREEN (PC3) pin config
Stml4::StGpioSettings dbg_green_settings{
    Stml4::GpioMode::OUTPUT, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams dbg_green_params{dbg_green_settings, 3, GPIOC};

// UART5
// UART5_TX (PC12) pin config
Stml4::StGpioSettings uart5_tx_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams uart5_tx_params{uart5_tx_settings, 12, GPIOC};
// UART5_RX (PD2) pin config
Stml4::StGpioSettings uart5_rx_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams uart5_rx_params{uart5_rx_settings, 2, GPIOD};


Stml4::HwGpio gpio_lmt_swt(lmt_swt_params);
Stml4::HwGpio gpio_rs_en(rs_en_params);
Stml4::HwGpio gpio_mtr_dir2(mtr_dir2_params);

Board board {.pps_board = {
    .gpio_lmt_swt = gpio_lmt_swt,
    .gpio_rs_en = gpio_rs_en,
    .gpio_mtr_dir2 = gpio_mtr_dir2}
}

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;

    // Initialize GPIOs
    bool ret = true;

    ret = ret && gpio_lmt_swt.init();
    ret = ret && gpio_rs_en.init();
    ret = ret && gpio_mtr_dir2.init();

    return ret;
}

Board& get_board()
{
    return board;
}
}  // namespace LBR