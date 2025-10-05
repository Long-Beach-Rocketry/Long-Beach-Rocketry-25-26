#pragma once

#include "gpio.h"
#include "st_gpio.h"
#include "st_usart.h"
#include "usart.h"

StUsart& CreateUsart(uint32_t sys_clck,
                     uint32_t baud_rate,
                     USART_TypeDef* base_addr,
                     LBR::Stml4::StGpioSettings rx_config,
                     LBR::Stml4::StGpioSettings tx_config,
                     uint8_t rx_pin,
                     uint8_t tx_pin,
                     GPIO_TypeDef* rx_base_addr,
                     GPIO_TypeDef* tx_base_addr);

StUsart& GiveUsart(uint32_t sys_clck,
                   uint32_t baud_rate,
                   USART_TypeDef* base_addr,
                   LBR::Stml4::StGpioSettings rx_config,
                   LBR::Stml4::StGpioSettings tx_config,
                   uint8_t rx_pin,
                   uint8_t tx_pin,
                   GPIO_TypeDef* rx_base_addr,
                   GPIO_TypeDef* tx_base_addr);