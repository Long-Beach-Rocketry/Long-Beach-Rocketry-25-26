#pragma once

#include "spi.h"
#include "st_gpio.h"
#include "st_spi.h"

using namespace LBR;

Spi& BSP_Init(SPI_TypeDef* spi_instance);