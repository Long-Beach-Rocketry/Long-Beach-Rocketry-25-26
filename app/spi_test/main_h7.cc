/**
 * @file main_h7.cc
 * @brief Bare-metal SPI1 communication test on STM32H723.
 * @author Bex Saw, Kent Hong, Alex Pulido
 */

#include <array>
#include "board.h"

using namespace LBR;

int main(void)
{
    // Enable clocks and initialize SPI pins
    bsp_init();

    // Get struct of our ready to use Chip Select Pin and SPI object
    Board spi_board = get_board();

    // Send command byte 0x90 to read Manufacturer ID for the w25q
    std::array<uint8_t, 6> tx_buffer = {0x90, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Create an array of data to receive (Should see rx_buffer[4] = 239 and rx_buffer[5] = 23)
    std::array<uint8_t, 6> rx_buffer = {0};

    while (1)
    {
        // Drive CS Pin low to allow write
        spi_board.cs.cs_enable();
        // Loop write to SPI lines
        spi_board.spi.seq_transfer(tx_buffer, rx_buffer);
        // Drive CS Pin high to end write
        spi_board.cs.cs_disable();
    }

    return 0;
}