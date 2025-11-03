/**
 * @file main.cc
 * @brief Bare-metal SPI1 communication test on STM32L476.
 * @author Bex Saw
 */

#include <array>
#include "gpio_cs.h"
#include "spi.h"
#include "spi_app_bsp.h"
#include "st_gpio.h"
#include "stm32l476xx.h"

using namespace LBR;
using namespace LBR::Stml4;

int main(void)
{
    // Init struct of addresses for SPI and GPIO
    SPI_TypeDef* spi_instance = SPI1;
    GPIO_TypeDef* gpio_instance = GPIOA;

    // Get the ready to use SPI object
    Spi& spi1 = BSP_Init(spi_instance, gpio_instance);

    // Send command byte 0x90 to read Manufacturer ID for the w25q
    std::array<uint8_t, 4> tx_buffer = {0x90, 0x00, 0x00, 0x00};

    // Create an array of data to receive
    std::array<uint8_t, 2>
        rx_buffer;  // Should see rx_buffer[0] = 239 and rx_buffer[1] = 23

    StGpioSettings gpio_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                 GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
    StGpioParams cs_params{gpio_settings, 4, gpio_instance};
    static HwGpio cs_gpio{cs_params};
    cs_gpio.init();
    cs_gpio.set(1);  // Initialize CS high (idle)
    GpioChipSelect chip_select{cs_gpio};

    while (1)
    {
        chip_select.ChipSelectEnable();
        // Loop write to PA7
        spi1.Transfer(tx_buffer, rx_buffer);
        chip_select.ChipSelectDisable();
    }
}
