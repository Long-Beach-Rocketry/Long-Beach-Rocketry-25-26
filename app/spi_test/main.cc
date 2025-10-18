/**
 * @file main.cc
 * @brief Bare-metal SPI1 communication test on STM32L476.
 * @author Bex Saw
 */

#include "spi.h"
#include "spi_app_bsp.h"
#include "st_spi.h"
#include "stm32l476xx.h"

using namespace LBR::Stml4;

int main(void)
{
    SPI_TypeDef* spi_instance = SPI1;
    GPIO_TypeDef* gpio_instance = GPIOA;
    Spi& spi1 = BSP_Init(spi_instance, gpio_instance);

    uint8_t tx_buffer[5] = {10, 20, 30, 40, 50};
    size_t tx_len = sizeof(tx_buffer);

    // Cast to derived type to access parameterized Write function
    LBR::Stml4::HwSpi& hw_spi = static_cast<LBR::Stml4::HwSpi&>(spi1);

    while (1)
    {
        hw_spi.Write(tx_buffer, tx_len);
    }
}
