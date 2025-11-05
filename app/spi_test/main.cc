/**
 * @file main.cc
 * @brief Bare-metal SPI1 communication test on STM32L476.
 * @author Bex Saw, Kent Hong
 */

#include <array>
#include "board.h"
#include "gpio_cs.h"
//#include "stm32l476xx.h"

using namespace LBR;

int main(void)
{
    // Enable clocks and initialize SPI pins
    BSP_Init();

    // Get struct of our ready to use Chip Select Pin and SPI object
    Board spi_board = Get_Board();

    // Send command byte 0x90 to read Manufacturer ID for the w25q
    std::array<uint8_t, 4> tx_buffer = {0x90, 0x00, 0x00, 0x00};

    // Create an array of data to receive (Should see rx_buffer[0] = 239 and rx_buffer[1] = 23)
    std::array<uint8_t, 2> rx_buffer;

    // Create Chip Select Object to manual toggle cs pin
    GpioChipSelect chip_select{spi_board.cs};

    while (1)
    {
        // Drive CS Pin low to allow write
        chip_select.ChipSelectEnable();
        // Loop write to PA7
        spi_board.spi1.Transfer(tx_buffer, rx_buffer);
        // Drive CS Pin high to end write
        chip_select.ChipSelectDisable();
    }

    return 0;
}
