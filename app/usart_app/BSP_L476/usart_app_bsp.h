#pragma once

#include "st_usart.h"
#include "usart.h"

Usart& BSP_Init(USART_TypeDef* base_addr);

// Gpio& BSP_GPIO_Init(GPIO_TypeDef* base_addr, uint8_t pin, uint32_t mode,
//                     uint32_t pull, uint32_t speed, uint32_t af);