#include "board.h"
#include "st_gpio.h"
#include "st_sysclk.h"
#include "st_usart.h"

static constexpr uint32_t kSysclkHz{100'000'000};
static constexpr uint32_t kDefaultUsartKernelHz{64'000'000};
static constexpr uint32_t kBaudRate{115'200};

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
StUsartParams usart_params{USART3, kDefaultUsartKernelHz, kBaudRate};
StUsart usart{usart_params};

// Create Clock object (Sysclk = 100 MHz, HCLK = 50 MHz, PCLK1 = PCLK2 = PCLK3 = PCLK4 = 25 MHz)
ClockParams clock_params{Source::HSI64_MHZ,     kSysclkHz,
                         D1cprePrescaler::DIV1, AhbPrescaler::DIV2,
                         Apb1Prescaler::DIV2,   Apb2Prescaler::DIV2,
                         Apb3Prescaler::DIV2,   Apb4Prescaler::DIV2};
HwClock clock{clock_params};
}  // namespace Stmh7

Board board{.usart = Stmh7::usart, .clock = Stmh7::clock};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    bool ret = true;

    // Initialize USART pins and USART
    ret &= Stmh7::usart_tx.init();
    ret &= Stmh7::usart_rx.init();
    ret &= Stmh7::usart.init();

    // Configure clock tree for this test.
    bool clock_ok = Stmh7::clock.init();
    ret &= clock_ok;

    // Retune BRR using the actual post-init APB1 frequency.
    if (clock_ok)
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

    // Keep USART3 IRQ disabled for this TX-only test app.
    NVIC_DisableIRQ(USART3_IRQn);

    return ret;
}

Board& get_board()
{
    return board;
}

extern "C" void USART3_IRQHandler(void)
{
    if (Stmh7::usart.get_addr()->ISR & USART_ISR_RXNE_RXFNE)
    {
        if (board.usart.receive(rxb))
        {
            // received 1 byte, echo back
            std::span<const uint8_t> tx_span(&rxb, 1);
            board.usart.send(tx_span);
        }
    }
}
}  // namespace LBR