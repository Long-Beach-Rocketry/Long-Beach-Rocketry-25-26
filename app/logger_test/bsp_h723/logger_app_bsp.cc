#include "board.h"
#include "st_gpio.h"
#include "st_sysclk.h"
#include "st_usart.h"

using namespace LBR::Stmh7;

static constexpr uint32_t kSysclkHz{8'000'000};
static constexpr uint32_t kBaudRate{115'200};

namespace LBR
{
namespace Stmh7
{

StGpioSettings tx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};

StGpioSettings rx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};

StGpioParams tx_params = {tx_config, (uint8_t)8, GPIOD};
StGpioParams rx_params = {rx_config, (uint8_t)9, GPIOD};

HwGpio tx_gpio(tx_params);
HwGpio rx_gpio(rx_params);

// Default frequency for the H7 (HSI / 1), (per Datasheet 8.7.2)
StUsartParams usart_params = {USART3, 64000000, 115200};

StUsart usart(usart_params);

// Create Clock object (Sysclk = 65 MHz, HCLK = 32.5 MHz, PCLK1 = PCLK2 = PCLK3 = PCLK4 = 16.25 MHz)
ClockParams clock_params{Source::HSE8_MHZ_BYPASS, kSysclkHz,
                         D1cprePrescaler::DIV1,   AhbPrescaler::DIV2,
                         Apb1Prescaler::DIV2,     Apb2Prescaler::DIV2,
                         Apb3Prescaler::DIV2,     Apb4Prescaler::DIV2};
HwClock clock{clock_params};
}  // namespace Stmh7

Board board{.usart = Stmh7::usart,
            .rx = Stmh7::rx_gpio,
            .tx = Stmh7::tx_gpio,
            .clock = Stmh7::clock};

bool bsp_init()
{
    bool result = true;

    // Enable GPIOD for pins PD8, PD9
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;

    // Enable USART3 clock
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    result &= tx_gpio.init();

    result &= rx_gpio.init();

    result &= usart.init();

    // Retune BRR using the actual post-init APB1 frequency.
    if (clock.init())
    {
        const uint32_t apb1_hz = Stmh7::clock.get_clock_frequencies().apb1;
        USART_TypeDef* usart_addr = Stmh7::usart.get_addr();

        // Reconfigure BRR after clock switch and explicitly keep TX enabled.
        usart_addr->CR1 &= ~USART_CR1_UE;
        usart_addr->BRR = apb1_hz / kBaudRate;
        usart_addr->CR1 |= USART_CR1_TE | USART_CR1_RE;

        // sysclk_test is TX-only; disable RX interrupts to avoid echo/noise IRQ spam.
        usart_addr->CR1 &= ~USART_CR1_RXNEIE_RXFNEIE;
        usart_addr->CR1 |= USART_CR1_UE;
    }
    else
    {
        result = false;
    }

    // Keep USART3 IRQ disabled for this TX-only test app.
    NVIC_DisableIRQ(USART3_IRQn);
    return result;
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

}  // namespace LBR