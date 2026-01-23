/**
 * @file main.cc
 * @author Kent Hong
 * @brief SPI test on the W25Q flash chip
 * 
 */

#include <array>
#include "board.h"
#include "w25q.h"

using namespace LBR;

int main(void)
{
    /* Enable clocks and initialize SPI pins */
    bsp_init();

    /* Get struct of our ready to use Chip Select Pin and SPI object */
    Board spi_board = get_board();

    /* Create W25Q obj */
    W25q flash{spi_board.spi1, spi_board.cs};

    /* Verify reset (status should be true indicating WEL was cleared) */
    [[maybe_unused]] bool status = flash.reset();

    /* Verify init */
    std::array<uint8_t, 1> status_reg_val;
    status = flash.status_reg_read(W25q::StatusRead::STATUS_REGISTER_3,
                                   status_reg_val);
    status = flash.init();
    // status_reg_val should have bit 2 set (if we're assuming 0th bit for first bit)
    status = flash.status_reg_read(W25q::StatusRead::STATUS_REGISTER_3,
                                   status_reg_val);

    /* Verify BusyCheck, write_enable, status_reg_write, and status_reg_read */
    status = flash.status_reg_write(W25q::StatusWrite::STATUS_REGISTER_1,
                                    (1 << 2), (1 << 2));
    // status_reg_val should be 0x04
    status = flash.status_reg_read(W25q::StatusRead::STATUS_REGISTER_1,
                                   status_reg_val);
    status = flash.status_reg_write(W25q::StatusWrite::STATUS_REGISTER_1,
                                    (1 << 2), (0 << 2));
    // status_reg_val should be 0x00
    status = flash.status_reg_read(W25q::StatusRead::STATUS_REGISTER_1,
                                   status_reg_val);

    /* Verify page_program and Read */
    std::array<uint8_t, 3> txbuf{0x02u, 0x04u, 0x06u};
    std::array<uint8_t, 3> rxbuf{};
    status = flash.page_program(1, 1, 1, 0, txbuf, rxbuf);
    // After page_program, rxbuf should hold the same values as txbuf

    /* Verify block_erase */
    status = flash.block_erase(1);
    // rxbuf should be 0xFF
    status = flash.read(1, 1, 1, 0, rxbuf);

    /* Verify sector_erase */
    std::array<uint8_t, 1> txbuf2{0x08u};
    std::array<uint8_t, 1> rxbuf2{};
    status = flash.page_program(0, 1, 0, 0, txbuf2, rxbuf2);
    // rxbuf2 should be 0x08 after page_program
    status = flash.sector_erase(0, 1);
    status = flash.read(0, 1, 0, 0, rxbuf2);
    // rxbuf2 should be 0xFF after sector_erase and Read

    /* Verify chip_erase */
    status = flash.page_program(0, 1, 0, 0, txbuf2, rxbuf2);
    // rxbuf2 should be 0x08 after page_program
    status = flash.chip_erase();
    status = flash.read(0, 1, 0, 0, rxbuf2);
    // rxbuf2 should be 0xFF after chip_erase and Read

    /* Verify block_lock and block_unlock */
    status = flash.block_lock(1);
    status = flash.block_unlock(1);
    // Both these functions should return true for status

    while (1)
    {
    }

    return 0;
}