#include "board.h"
#include "st_gpio.h"
#include "st_sysclk.h"
#include "st_timebase.h"
#include "st_usart.h"

static constexpr uint32_t kSysclkHz{8'000'000};
static constexpr uint32_t kBaudRate{115'200};
static constexpr uint32_t kUsart3ClockHz{2'000'000};
static constexpr uint32_t kTim2ClockHz{4'000'000};

namespace LBR
{
namespace Stmh7
{

// Use ST-Link VCP pins for USART3 on H723 boards (COM port output)
StGpioSettings usart_settings{GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                              GpioOspeed::LOW, GpioPupd::NO_PULL, 7};
StGpioParams usart_tx_params{usart_settings, 8, GPIOD};
StGpioParams usart_rx_params{usart_settings, 9, GPIOD};
HwGpio usart_tx{usart_tx_params};
HwGpio usart_rx{usart_rx_params};

// Create USART3 object
StUsartParams usart_params{USART3, kUsart3ClockHz, kBaudRate};
StUsart usart{usart_params};

// Create Clock object (Sysclk = 65 MHz, HCLK = 32.5 MHz, PCLK1 = PCLK2 = PCLK3 = PCLK4 = 16.25 MHz)
ClockParams clock_params{Source::HSE8_MHZ_BYPASS, kSysclkHz,
                         D1cprePrescaler::DIV1,   AhbPrescaler::DIV2,
                         Apb1Prescaler::DIV2,     Apb2Prescaler::DIV2,
                         Apb3Prescaler::DIV2,     Apb4Prescaler::DIV2};
HwClock clock{clock_params};

// Create Timebase object (TIM2, 16.25 MHz input, enable IRQ)
StTimebaseParams timebase_params{TIM2, TIM2_IRQn, kTim2ClockHz, true};
HwTimebase timebase{timebase_params};

}  // namespace Stmh7

Board board{.usart = Stmh7::usart,
            .clock = Stmh7::clock,
            .timebase = Stmh7::timebase};

bool bsp_init()
{
    // Enable peripheral clocks for GPIOD, USART3, and TIM2
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;
    RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;

    bool ret = true;

    ret &= Stmh7::clock.init();
    ret &= Stmh7::usart_tx.init();
    ret &= Stmh7::usart_rx.init();
    ret &= Stmh7::usart.init();

    // TIM2 must be initialized using its actual post-clock-switch frequency.
    ret &= Stmh7::timebase.init();

    NVIC_DisableIRQ(USART3_IRQn);

    return ret;
}

Board& get_board()
{
    return board;
}

extern "C" void IncDelayTicks(void);

extern "C" void SysTick_Handler(void)
{
    HAL_IncTick();
    IncDelayTicks();
}

extern "C" void TIM2_IRQHandler(void)
{
    Stmh7::timebase.handle_irq();
}

}  // namespace LBR