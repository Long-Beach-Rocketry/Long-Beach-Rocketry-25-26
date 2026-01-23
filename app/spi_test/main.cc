/**
 * @file main.cc
 * @brief Bare-metal SPI1 communication test on STM32L476.
 * @author Bex Saw, Kent Hong
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
    std::array<uint8_t, 4> tx_buffer = {0x90, 0x00, 0x00, 0x00};

    // Create an array of data to receive (Should see rx_buffer[0] = 239 and rx_buffer[1] = 23)
    std::array<uint8_t, 2> rx_buffer;

    while (1)
    {
        // Drive CS Pin low to allow write
        spi_board.cs.cs_enable();
        // Loop write to PA7
        spi_board.spi1.seq_transfer(tx_buffer, rx_buffer);
        // Drive CS Pin high to end write
        spi_board.cs.cs_disable();
    }

    return 0;
}
