#include "board.h"
#include "st_crc.h"
#include "st_gpio.h"
#include "st_usart.h"

using namespace LBR::Stmh7;

namespace LBR
{

// CRC-32/BZIP2
static constexpr uint32_t kPoly{0x4C11DB7};
static constexpr uint32_t kInit{CRC_INIT_INIT};
static constexpr uint32_t kXorOut{0xFFFFFFFF};

StCrcSettings crc_settings{CrcRevOut::NO_REV, CrcRevIn::NO_REV,
                           CrcPolySize::BIT_SIZE_32};
StCrcParams crc_params{crc_settings, CRC, kInit, kPoly, kXorOut};
HwCrc crc{crc_params};

StGpioSettings tx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};
StGpioSettings rx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};

StGpioParams tx_params = {tx_config, (uint8_t)8, GPIOD};
StGpioParams rx_params = {rx_config, (uint8_t)9, GPIOD};

HwGpio tx_gpio{tx_params};
HwGpio rx_gpio{rx_params};

StUsartParams usart_params = {USART3, 64000000, 115200};
StUsart usart{usart_params};

Pipeline pipeline{crc};

uint8_t rxb;

Board board{.usart = usart,
            .rx = rx_gpio,
            .tx = tx_gpio,
            .crc = crc,
            .pipeline = pipeline};

bool bsp_init()
{
    bool ret = true;

    RCC->AHB4ENR |= RCC_AHB4ENR_CRCEN | RCC_AHB4ENR_GPIODEN;
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    /* Timestamp in microsecond specific configs */

    // Enable DWT cycle counter for hardware microsecond timestamps
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // Configure SysTick for 1 ms timebase so Utils::DelayMs() works
    SysTick_Config(SystemCoreClock / 1000);

    /* Will strip away when testing is good */

    ret &= crc.init();
    ret &= tx_gpio.init();
    ret &= rx_gpio.init();
    ret &= usart.init();

    // No NVIC for USART3 — pipeline.receive() polls the USART directly.
    // Enabling the ISR here would consume RX bytes before poll_usart() can see them.

    return ret;
}

/* I will make a helper and throw this in utils (delay.h) */
/* The current function get timestamp in microseconds */
uint32_t get_us()
{
    // DWT->CYCCNT wraps at ~67s @ 64 MHz — fine for loopback timing
    return DWT->CYCCNT / (SystemCoreClock / 1000000UL);
}

Board& get_board()
{
    return board;
}

extern "C" void IncDelayTicks(void);

extern "C" void SysTick_Handler(void)
{
    IncDelayTicks();
}

}  // namespace LBR
