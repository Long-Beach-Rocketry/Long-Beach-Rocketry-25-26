/**
 * @file main.cc
 * @brief Bare-metal SPI1 communication test on STM32L476.
 * @author Bex Saw
 */

#include "cs_gpio.h"
#include "spi.h"
#include "spi_app_bsp.h"
#include "st_gpio.h"
#include "stm32l476xx.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

using namespace LBR;
using namespace LBR::Stml4;

int main(void)
{
    // Per-board peripheral base pointers
    SPI_TypeDef* spi_instance = SPI1;
    GPIO_TypeDef* gpio_instance = GPIOA;

    // Initialize BSP and get generic interface, then cast to concrete
    Spi& spi_iface = BSP_Init(spi_instance, gpio_instance);
    auto& hw_spi = static_cast<LBR::Stml4::HwSpi&>(spi_iface);

    // Configure chip-select GPIO (PA4 as example)
    StGpioSettings gpio_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                 GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
    StGpioParams cs_params{gpio_settings, 4, gpio_instance};
    static HwGpio cs_gpio{cs_params};
    cs_gpio.init();
    cs_gpio.set(true);  // idle high

    GpioChipSelect cs{cs_gpio};

    // Command to read JEDEC ID from SPI flash (example). Adjust as needed.
    const uint8_t tx_buf[] = {0x9F, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t rx_buf[sizeof(tx_buf)];
    std::memset(rx_buf, 0, sizeof(rx_buf));

    while (1)
    {
        cs.ChipSelectEnable();
        hw_spi.Transfer(tx_buf, rx_buf, sizeof(tx_buf));
        cs.ChipSelectDisable();
    }

    return 0;
}