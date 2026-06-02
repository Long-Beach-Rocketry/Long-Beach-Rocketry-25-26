#include "board.h"
#include "st_gpio.h"
#include "st_sysclk.h"
#include "st_usart.h"

static constexpr uint32_t kSysclkHz{100'000'000};
static constexpr uint32_t kBaudRate{115'200};

namespace LBR
{
namespace Stmh7
{

// Create USART3 TX and RX pins
StGpioSettings usart_settings{GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                              GpioOspeed::LOW, GpioPupd::NO_PULL, 7};
StGpioParams usart_tx_params{usart_settings, 10, GPIOB};
StGpioParams usart_rx_params{usart_settings, 11, GPIOB};
HwGpio usart_tx{usart_tx_params};
HwGpio usart_rx{usart_rx_params};

// Create USART3 object
StUsartParams usart_params{USART3, kSysclkHz, kBaudRate};
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
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    // Initialize USART pins and USART
    bool ret = true;

    ret = ret && Stmh7::usart_tx.init();
    ret = ret && Stmh7::usart_rx.init();
    ret = ret && Stmh7::usart.init();

    // Initialize clock
    ret = ret && Stmh7::clock.init();

    // Enable interrupts for USART3
    NVIC_SetPriority(USART3_IRQn, 0);  // priority (0 being highest)
    NVIC_EnableIRQ(USART3_IRQn);

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