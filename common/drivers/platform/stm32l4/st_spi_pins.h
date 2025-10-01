// !: This is probably not going to be used, but I'm leaving it here in case.

/**
 * @file st_spi_pins.h
 * @author Kent Hong
 * @brief SPI Driver stm32l476xx pin map for nss, sck, miso, and mosi lines based on alternate function table of the datasheet
 * @version 0.1
 * @date 2025-09-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include "stm32l4xx.h"
#include <cstdint>

/**
 * @brief Desired Pin Mapping for creating mosi, miso, sck, and nss GPIO objects
 * 
 * @note Use this struct to create HwGpio objects for the nss, sck, miso, and mosi pins
 * 
 * @example Declaring a HwGpio object to make an SPI1 serial clock pin using PA5 Alternate Function 5:  
 *
 *          StGpioSettings sck_settings = CreateSpiPinSettings(SPI1_SCK[1].af)
 *          HwGpio spi1_sck{sck_settings, SPI1_SCK[1].pin, SPI1_SCK[1].port};
 * 
 */
struct SpiPinMap {
    GPIO_TypeDef* port;
    uint8_t pin;
    uint8_t af;
};

/**
 * @brief SPI1 Pin Tables from the Alternate function tables in the stm32l476 datasheet
 * 
 */
static constexpr SpiPinMap SPI1_NSS[] = {
    {GPIOA, 4, 5},
    {GPIOA, 15, 5},
    {GPIOE, 12, 5},
    {GPIOG, 5, 5}
}

static constexpr SpiPinMap SPI1_SCK[] = {
    {GPIOA, 5, 5},
    {GPIOB, 3, 5},
    {GPIOE, 13, 5},
    {GPIOG, 2, 5}
}

static constexpr SpiPinMap SPI1_MISO[] = {
    {GPIOA, 6, 5},
    {GPIOB, 4, 5},
    {GPIOE, 14, 5},
    {GPIOG, 3, 5}
}

static constexpr SpiPinMap SPI1_MOSI[] = {
    {GPIOA, 7, 5},
    {GPIOB, 5, 5},
    {GPIOE, 15, 5},
    {GPIOG, 4, 5}
}

/**
 * @brief SPI2 Pin Tables from the Alternate function tables in the stm32l476 datasheet
 * 
 */
static constexpr SpiPinMap SPI2_NSS[] = {
    {GPIOB, 9, 5},
    {GPIOB, 12, 5},
    {GPIOD, 0, 5}
}

static constexpr SpiPinMap SPI2_SCK[] = {
    {GPIOB, 10, 5},
    {GPIOB, 13, 5},
    {GPIOD, 1, 5}
}

static constexpr SpiPinMap SPI2_MISO[] = {
    {GPIOB, 14, 5},
    {GPIOC, 2, 5},
    {GPIOD, 3, 5}
}

static constexpr SpiPinMap SPI2_MOSI[] = {
    {GPIOB, 15, 5},
    {GPIOC, 3, 5},
    {GPIOD, 4, 5}
}

/**
 * @brief SPI3 Pin Tables from the Alternate function tables in the stm32l476 datasheet
 * 
 */
static constexpr SpiPinMap SPI3_NSS[] = {
    {GPIOA, 4, 6},
    {GPIOA, 15, 6},
    {GPIOG, 12, 6}
}

static constexpr SpiPinMap SPI3_SCK[] = {
    {GPIOB, 3, 6},
    {GPIOC, 10, 6},
    {GPIOG, 9, 6}
}

static constexpr SpiPinMap SPI3_MISO[] = {
    {GPIOB, 4, 6},
    {GPIOC, 11, 6},
    {GPIOG, 10, 6}
}

static constexpr SpiPinMap SPI3_MOSI[] = {
    {GPIOB, 5, 6},
    {GPIOC, 12, 6},
    {GPIOG, 11, 6}
}

