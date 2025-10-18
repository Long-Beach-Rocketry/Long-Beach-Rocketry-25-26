#pragma once

#include "spi.h"
#include "stm32l476xx.h"

using namespace LBR;

Spi& BSP_Init(SPI_TypeDef* spi_instance, GPIO_TypeDef* gpio_instance);