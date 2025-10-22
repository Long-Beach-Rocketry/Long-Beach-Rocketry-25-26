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
    // BSP handles the hardware setup and returns an initialized SPI handle
    LBR::Spi& spi1 = BSP_Init(SPI1, GPIOA);

    uint8_t tx_buffer[5] = {10, 20, 30, 40, 50};
    size_t tx_len = sizeof(tx_buffer);

    auto& hw_spi = static_cast<LBR::Stml4::HwSpi&>(spi1);

    while (1)
    {
        hw_spi.Write(tx_buffer, tx_len);

        hw_spi.Read(tx_buffer, tx_len);
        
    }
}
