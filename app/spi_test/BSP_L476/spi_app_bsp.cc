#include "spi_app_bsp.h"

using namespace LBR::Stml4;

static HwSpi spi1(SPI1, {
    .baudRate    = SpiBaudRate::FPCLK_16,
    .busMode     = SpiBusMode::MODE1,
    .dataSize    = SpiDataSize::DATA_8BIT,
    .bitOrder    = SpiBitOrder::MSB_FIRST,
    .rxThreshold = SpiRxThreshold::RX_8BIT
});

Spi& BSP_Init(SPI_TypeDef* base_addr)
{
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
