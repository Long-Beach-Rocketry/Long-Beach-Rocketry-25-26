#pragma once
#include "gpio.h"
#include "spi.h"
#include "stm32l476xx.h"
//do the cmake

namespace LBR
{

struct Board
{
    Gpio& clk;
    Gpio& miso;
    Gpio& mosi;
    Gpio& cs;
    Spi& spi;
};

Spi& BSP_Init(SPI_TypeDef* spi_instance, GPIO_TypeDef* gpio_instance);

}  // namespace LBR