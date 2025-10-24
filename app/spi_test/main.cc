/**
 * @file main.cc
 * @brief Bare-metal SPI1 communication test on STM32L476.
 * @author Bex Saw
 */

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
    uint8_t tx_buffer[6] = {0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    // Get the size of the array
    size_t tx_len = sizeof(tx_buffer);

    // Create an array of data to receive
    uint8_t rx_buffer[6];
    //size_t rx_len = sizeof(rx_buffer);

    StGpioSettings gpio_settings{GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                                 GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 5};
    StGpioParams cs_params{gpio_settings, 4, gpio_instance};
    static HwGpio cs_gpio{cs_params};
    cs_gpio.init();
    GpioChipSelect chip_select{cs_gpio};

    while (1)
    {
        chip_select.ChipSelectEnable();
        // Loop write to PA7
        spi1.Transfer(tx_buffer, rx_buffer, tx_len);
        chip_select.ChipSelectDisable();
    }
}
