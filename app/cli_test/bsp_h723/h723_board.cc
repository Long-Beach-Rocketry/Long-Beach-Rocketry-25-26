#include "board.h"
#include "st_gpio.h"
#include "st_usart.h"

static constexpr uint32_t kSysclkHz{64'000'000};
static constexpr uint32_t kBaudRate{115'200};

namespace LBR
{
namespace Stmh7
{
// Configure LED pins
StGpioSettings ld_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                           GpioOspeed::LOW, GpioPupd::NO_PULL, 0};
StGpioParams ld1_params{ld_settings, 0, GPIOB};
StGpioParams ld2_params{ld_settings, 1, GPIOE};
StGpioParams ld3_params{ld_settings, 14, GPIOB};

HwGpio ld1{ld1_params};
HwGpio ld2{ld2_params};
HwGpio ld3{ld3_params};

// Pins for USART

StGpioSettings tx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};

StGpioSettings rx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};

StGpioParams tx_params = {tx_config, (uint8_t)8, GPIOD};
StGpioParams rx_params = {rx_config, (uint8_t)9, GPIOD};

HwGpio tx_gpio(tx_params);
HwGpio rx_gpio(rx_params);

// Create USART3 object
StUsartParams usart_params{USART3, kSysclkHz, kBaudRate};
StUsart usart{usart_params};
}  // namespace Stmh7

Board board{.usart = Stmh7::usart,
            .led1 = Stmh7::ld1,
            .led2 = Stmh7::ld2,
            .led3 = Stmh7::ld3};

bool board_init()
{
    bool result = true;

    // Enable GPIOB and GPIOE LED clock
    RCC->AHB4ENR |= (RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIOEEN);

    // Enable USART3 clock
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    result &= Stmh7::ld1.init();
    result &= Stmh7::ld2.init();
    result &= Stmh7::ld3.init();

    result &= Stmh7::usart.init();

    // Nested Vectored Interrupt Controller (NVIC) - a hardware block that sits between peripherals and CPU.
    // Manages every interrupt in the system.
    NVIC_SetPriority(USART3_IRQn, 0);  // priority (0 being highest)

    // Tells NVIC to start listening for USART3 interrupt requests.
    NVIC_EnableIRQ(USART3_IRQn);

    return result;
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
