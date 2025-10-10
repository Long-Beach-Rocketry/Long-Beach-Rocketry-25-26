#include "spi_app_bsp.h"
#include "stm32l476xx.h"

using namespace LBR::Stml4;

// Define SPI configuration
static StSpiSettings spi_cfg = {
    .baudrate   = SpiBaudRate::FPCLK_16,
    .busmode    = SpiBusMode::MODE1,
    .order      = SpiBitOrder::MSB,
    .threshold  = SpiRxThreshold::FIFO_8bit
};

// Create SPI1 instance
static HwSpi spi1(SPI1, spi_cfg);

// Initialize SPI BSP
Spi& BSP_Init(SPI_TypeDef* base_addr)
{
    (void)base_addr; // avoid unused parameter warning

    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Configure pins PA5(SCK), PA6(MISO), PA7(MOSI)
    GPIOA->MODER &= ~(GPIO_MODER_MODE5_Msk | GPIO_MODER_MODE6_Msk | GPIO_MODER_MODE7_Msk);
    GPIOA->MODER |=  (0b10 << GPIO_MODER_MODE5_Pos)
                  |  (0b10 << GPIO_MODER_MODE6_Pos)
                  |  (0b10 << GPIO_MODER_MODE7_Pos);

    GPIOA->AFR[0] &= ~((0xF << GPIO_AFRL_AFSEL5_Pos)
                    |   (0xF << GPIO_AFRL_AFSEL6_Pos)
                    |   (0xF << GPIO_AFRL_AFSEL7_Pos));
    GPIOA->AFR[0] |=  (5 << GPIO_AFRL_AFSEL5_Pos)
                   |   (5 << GPIO_AFRL_AFSEL6_Pos)
                   |   (5 << GPIO_AFRL_AFSEL7_Pos);

    // Enable SPI1 clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    spi1.Init();
    return spi1;
}
