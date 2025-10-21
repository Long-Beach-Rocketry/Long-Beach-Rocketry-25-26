#pragma once

#include "spi.h"
#include "st_gpio.h"
#include "st_spi.h"
#include "stm32l476xx.h"

using namespace LBR;

// Use the STM32 HAL type name for GPIO structure
Spi& BSP_Init(SPI_TypeDef* spi_instance, GPIO_TypeDef* gpio_instance);