#pragma once

#include "gpio.h"
#include "st_gpio.h"
#include "st_usart.h"
#include "usart.h"

Usart& UsartModule(StUsartParams& usart_params, LBR::Stml4::StGpioParams& rx,
                   LBR::Stml4::StGpioParams& tx);