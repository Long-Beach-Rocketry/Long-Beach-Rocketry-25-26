/**
 * @file    main.cc
 * @author  Bex Saw
 * @brief   Bare-metal SPI test for STM32L4 using HwSpi driver
 * @version 0.3
 * @date    2025-10-05
 */

#include "st_spi.h"
#include "st_spi_settings.h"
#include "stm32l4xx.h"

using namespace LBR::Stml4;

int main(void)
{

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;   // Enable SPI1 clock

    // SPI1 pins (PA5=SCK, PA6=MISO, PA7=MOSI)
    GPIOA->MODER &=
        ~(GPIO_MODER_MODE5_Msk | GPIO_MODER_MODE6_Msk | GPIO_MODER_MODE7_Msk);
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODE5_Pos) |
                    (0b10 << GPIO_MODER_MODE6_Pos) |
                    (0b10 << GPIO_MODER_MODE7_Pos);

    // For Alternate Function mode, set AF5 for SPI1
    GPIOA->AFR[0] &=
        ~((0xF << GPIO_AFRL_AFSEL5_Pos) | (0xF << GPIO_AFRL_AFSEL6_Pos) |
          (0xF << GPIO_AFRL_AFSEL7_Pos));
    GPIOA->AFR[0] |= (5 << GPIO_AFRL_AFSEL5_Pos) | (5 << GPIO_AFRL_AFSEL6_Pos) |
                     (5 << GPIO_AFRL_AFSEL7_Pos);

    // Highspeed
    GPIOA->OSPEEDR |= (0b11 << GPIO_OSPEEDR_OSPEED5_Pos) |
                      (0b11 << GPIO_OSPEEDR_OSPEED6_Pos) |
                      (0b11 << GPIO_OSPEEDR_OSPEED7_Pos);

    // 3. Configure SPI settings
    StSpiSettings spiSettings{SpiBaudRate::FPCLK_8,
                              SpiBusMode::MODE2,  // Assume CPOL=0, CPHA=1
                              SpiDataSize::DATA_8BIT, SpiBitOrder::MSB_FIRST,
                              SpiRxThreshold::RX_8BIT};

    HwSpi spi(SPI1, spiSettings);

    if (spi.Init() != SpiStatus::OK)
    {
        // Initialization failed, loop in here
        while (1)
        {
            __BKPT(0);
        }
    }

    // 4. TXFIFO RXFIFO loopback: PA7 -> PA6
    uint8_t txData[] = {0xAA, 0x55};
    uint8_t rxData[2] = {0};

    spi.Send(txData, 2, 1000);
    spi.Read(rxData, 2, 1000);

    // Validation: verify data
    bool match = true;
    for (size_t i = 0; i < sizeof(txData); ++i)
    {
        if (rxData[i] != txData[i])
        {
            match = false;
            break;
        }
    }

    // Superloop for Validation
    while (1)
    {
        if (match)
        {
            __NOP();  // success → idle or probe with logic analyzer
        }
        else
        {
            __BKPT(0);  // data mismatch → debugger breakpoint
        }
    }
}
