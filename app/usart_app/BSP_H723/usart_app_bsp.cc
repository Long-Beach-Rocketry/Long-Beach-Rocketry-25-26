#include "board.h"
#include "st_gpio.h"
#include "st_usart.h"

using namespace LBR::Stmh7;

namespace LBR
{

StGpioSettings tx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};

StGpioSettings rx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};

StGpioParams tx_params = {tx_config, (uint8_t)8, GPIOD};
StGpioParams rx_params = {rx_config, (uint8_t)9, GPIOD};

HwGpio tx_gpio(tx_params);
HwGpio rx_gpio(rx_params);

// Make clock
// Stmh7::HwClock clock{};

// Default frequency for the H7 (HSI / 1), (per Datasheet 8.7.2)
StUsart usart(USART3, 64000000, 115200);

Board board{.usart = usart, .tx = tx_gpio, .rx = rx_gpio};

bool bsp_init()
{
    bool result = true;

    // Enable GPIOD for pins PD8, PD9
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;

    // Enable USART3 clock
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    result &= usart.init();

    result &= tx_gpio.init();

    result &= rx_gpio.init();

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
    if (usart.get_addr()->ISR & USART_ISR_RXNE_RXFNE)
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