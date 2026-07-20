#include "board.h"
#include "st_gpio.h"
#include "st_sysclk.h"
#include "st_timebase.h"
#include "st_usart.h"

static constexpr uint32_t kSysclkHz{8'000'000};
static constexpr uint32_t kBaudRate{115'200};
static constexpr uint32_t kUsart3ClockHz{2'000'000};
// TIM2 (32-bit) and TIM3 (16-bit) are both on APB1, so both see the same
// kernel clock (2 x PCLK1 = 4 MHz with the current clock config).
static constexpr uint32_t kTim2ClockHz{4'000'000};
static constexpr uint32_t kTim3ClockHz{4'000'000};

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

// Create Clock object (Sysclk = 8 MHz, HCLK = 4 MHz, PCLK1 = PCLK2 = PCLK3 = PCLK4 = 2 MHz)
ClockParams clock_params{Source::HSE8_MHZ_BYPASS, kSysclkHz,
                         D1cprePrescaler::DIV1,   AhbPrescaler::DIV2,
                         Apb1Prescaler::DIV2,     Apb2Prescaler::DIV2,
                         Apb3Prescaler::DIV2,     Apb4Prescaler::DIV2};
HwClock clock{clock_params};

// User LED LD1 (green) on PB0 - toggled by the timebase blink test
StGpioSettings led_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0};
StGpioParams led_params{led_settings, 0, GPIOB};
HwGpio led{led_params};

StTimebaseParams timebase_params{TIM3, TIM3_IRQn, kTim3ClockHz, true};
HwTimebase timebase{timebase_params};

}  // namespace Stmh7

Board board{.usart = Stmh7::usart,
            .clock = Stmh7::clock,
            .timebase = Stmh7::timebase,
            .led = Stmh7::led};

bool bsp_init()
{
    // Enable peripheral clocks for GPIOD & GPIOB, USART3
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN | RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    // Enable peripheral clocks for TIM2 or TIM3
    // RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
    RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;

    bool ret = true;

    ret &= Stmh7::clock.init();
    ret &= Stmh7::usart_tx.init();
    ret &= Stmh7::usart_rx.init();
    ret &= Stmh7::led.init();
    ret &= Stmh7::usart.init();

    // NOTE:: 2 TIMER TESTS (check the correct IRQn_Type)

    // TIM2
    /* const IRQn_Type timebase_irqn = TIM2_IRQn; */

    // TIM3
    const IRQn_Type timebase_irqn = TIM3_IRQn;

    // NOTE:: For references (when we using timebase interface)
    // Keep the timer's NVIC line masked and set its priority before the timebase
    // driver touches the peripheral, so no overflow interrupt can fire while
    // the timer is being (re)configured. The driver itself no longer performs
    // any NVIC operations.
    NVIC_DisableIRQ(timebase_irqn);
    NVIC_ClearPendingIRQ(timebase_irqn);
    NVIC_SetPriority(timebase_irqn, 5U);

    // The timer must be initialized using its actual post-clock-switch frequency.
    ret &= Stmh7::timebase.init();

    // Timebase configured; clear any latched pending state and unmask the line.
    // This line exist solely b/c the timebase driver is not allowed to touch NVIC, so the BSP must do it
    NVIC_ClearPendingIRQ(timebase_irqn);
    NVIC_EnableIRQ(timebase_irqn);

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

/* The IRQ handler name must match the timer's vector-table entry. */

// extern "C" void TIM2_IRQHandler(void)
// {
//     Stmh7::timebase.handle_irq();
// }

extern "C" void TIM3_IRQHandler(void)
{
    Stmh7::timebase.handle_irq();
}

}  // namespace LBR