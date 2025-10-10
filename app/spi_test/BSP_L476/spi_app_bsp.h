#pragma once 

#include "st_gpio.h"
#include "st_spi.h"
#include "spi.h"

using namespace LBR;

Spi& BSP_Init(SPI_TypeDef* spi_instance);