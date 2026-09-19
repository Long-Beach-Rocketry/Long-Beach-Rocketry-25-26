#include <cstdint>
#include "board.h"
#include "st_crc.h"

#include "st_gpio.h"
#include "st_usart.h"

#include <random>

namespace LBR
{

/**
 * @note We can configure the crc according to some crc standard/algorithm.
 * Use https://ninjacalc.mbedded.ninja/calculators/software/crc-calculator as 
 * reference to see what to configure and the proper output.
 * 
 * Or https://crccalc.com/?crc=12345678&method=&datatype=hex&outtype=hex
 * 
 * Current crc standard for the test: CRC-32/BZIP2
 */

static constexpr uint32_t kDefaultPoly{0x4C11DB7};
static constexpr uint32_t kCrcValue{CRC_INIT_INIT};
static constexpr uint32_t kCrcXOROut{0xFFFFFFFF};

// Crc config
Stmh7::StCrcSettings crc_settings{Stmh7::CrcRevOut::NO_REV,
                                  Stmh7::CrcRevIn::NO_REV,
                                  Stmh7::CrcPolySize::BIT_SIZE_32};

const Stmh7::StCrcParams crc_params{crc_settings, CRC, kCrcValue, kDefaultPoly,
                                    kCrcXOROut};

// Create Crc object
Stmh7::HwCrc crc(crc_params);

// Usart config

Stmh7::StGpioSettings tx_config = {
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::PUSH_PULL,
    Stmh7::GpioOspeed::LOW, Stmh7::GpioPupd::NO_PULL, 0x7};

Stmh7::StGpioSettings rx_config = {
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::PUSH_PULL,
    Stmh7::GpioOspeed::LOW, Stmh7::GpioPupd::NO_PULL, 0x7};

Stmh7::StGpioParams tx_params = {tx_config, (uint8_t)8, GPIOD};
Stmh7::StGpioParams rx_params = {rx_config, (uint8_t)9, GPIOD};

Stmh7::HwGpio tx_gpio(tx_params);
Stmh7::HwGpio rx_gpio(rx_params);

// Default frequency for the H7 (HSI / 1), (per Datasheet 8.7.2)
Stmh7::StUsartParams usart_params = {USART3, 64000000, 115200};

Stmh7::StUsart usart(usart_params);

Board board{.crc = crc, .usart = usart, .rx = rx_gpio, .tx = tx_gpio};

bool bsp_init()
{
    // Enable crc & pin clock
    RCC->AHB4ENR |= RCC_AHB4ENR_CRCEN | RCC_AHB4ENR_GPIODEN;

    // Enable USART3 clock
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    // Initialize Crc and pins
    bool ret = true;

    ret = ret && crc.init();

    ret &= tx_gpio.init();

    ret &= rx_gpio.init();

    ret &= usart.init();

    // Nested Vectored Interrupt Controller (NVIC) - a hardware block that sits between peripherals and CPU.
    // Manages every interrupt in the system.
    NVIC_SetPriority(USART3_IRQn, 0);  // priority (0 being highest)

    // Tells NVIC to start listening for USART3 interrupt requests.
    NVIC_EnableIRQ(USART3_IRQn);

    return ret;
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

// Function to simulate noise and errors
std::vector<uint8_t> simulate_noise(std::span<const uint8_t> data,
                                    double dropProbability,
                                    double bitFlipProbability)
{
    static std::mt19937 gen(12345);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    std::vector<uint8_t> result;
    result.reserve(data.size());

    for (auto byte : data)
    {
        // Drop byte
        if (dis(gen) < dropProbability)
            continue;

        // Flip bits
        for (int i = 0; i < 8; i++)
        {
            if (dis(gen) < bitFlipProbability)
                byte ^= (1u << i);
        }

        result.push_back(byte);
    }

    return result;
}

}  // namespace LBR