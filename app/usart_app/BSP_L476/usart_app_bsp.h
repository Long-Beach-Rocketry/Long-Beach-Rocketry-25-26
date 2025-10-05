#pragma once

#include "st_gpio.h"
#include "st_usart.h"
#include "st_usart_factory.h"
#include "usart.h"

using namespace LBR;

Usart& BSP_Init(USART_TypeDef* base_addr);
