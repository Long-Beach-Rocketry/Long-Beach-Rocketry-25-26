#pragma once

#include "gpio_cs.h"
#include "spi.h"
#include "stm32l476xx.h"

using namespace LBR;

/**
 * @brief Sets up a ready to use SPI object for main.cc
 * 
 * @param spi_instance 
 * @param gpio_instance 
 * @return Spi& 
 */
Spi& BSP_Init(SPI_TypeDef* spi_instance, GPIO_TypeDef* gpio_instance);