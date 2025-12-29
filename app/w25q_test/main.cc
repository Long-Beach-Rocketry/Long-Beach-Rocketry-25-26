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
    BspInit();

    /* Get struct of our ready to use Chip Select Pin and SPI object */
    Board spi_board = GetBoard();

    /* Create W25Q obj */
    W25q flash{spi_board.spi1, spi_board.cs};

    /* Verify Reset (status should be true indicating WEL was cleared) */
    [[maybe_unused]] bool status = flash.Reset();

    /* Verify W25qInit */
    std::array<uint8_t, 1> status_reg_val;
    status = flash.StatusRegRead(W25q::StatusRead::STATUS_REGISTER_3,
                                 status_reg_val);
    status = flash.W25qInit();
    // status_reg_val should be 0x04
    status = flash.StatusRegRead(W25q::StatusRead::STATUS_REGISTER_3,
                                 status_reg_val);

    /* Verify BusyCheck, WriteEnable, StatusRegWrite, and StatusRegRead */
    status = flash.StatusRegWrite(W25q::StatusWrite::STATUS_REGISTER_1,
                                  (1 << 2), (1 << 2));
    status = flash.StatusRegRead(W25q::StatusRead::STATUS_REGISTER_1,
                                 status_reg_val);
    status = flash.StatusRegWrite(W25q::StatusWrite::STATUS_REGISTER_1,
                                  (1 << 2), (0 << 2));
    status = flash.StatusRegRead(W25q::StatusRead::STATUS_REGISTER_1,
                                 status_reg_val);

    /* Verify PageProgram and Read */
    std::array<uint8_t, 3> txbuf{0x02u, 0x04u, 0x06u};
    std::array<uint8_t, 3> rxbuf{};
    status = flash.PageProgram(1, 1, 1, 0, txbuf, rxbuf);
    status = flash.Read(1, 1, 1, 0, rxbuf);

    /* Verify BlockErase */
    status = flash.BlockErase(1);
    // rxbuf should be 0xFF
    status = flash.Read(1, 1, 1, 0, rxbuf);

    /* Verify SectorErase */
    std::array<uint8_t, 1> txbuf2{0x08u};
    std::array<uint8_t, 1> rxbuf2{};
    status = flash.PageProgram(0, 1, 0, 0, txbuf2, rxbuf2);
    // rxbuf2 should be 0x08
    status = flash.Read(0, 1, 0, 0, rxbuf2);
    status = flash.SectorErase(0, 1);
    // rxbuf2 should be 0xFF
    status = flash.Read(0, 1, 0, 0, rxbuf2);

    /* Verify ChipErase */
    status = flash.PageProgram(0, 1, 0, 0, txbuf2, rxbuf2);
    // rxbuf2 should be 0x08
    status = flash.Read(0, 1, 0, 0, rxbuf2);
    status = flash.ChipErase();
    // rxbuf2 should be 0xFF
    status = flash.Read(0, 1, 0, 0, rxbuf2);

    /* Verify BlockLock and BlockUnlock */
    status = flash.BlockLock(1);
    status = flash.BlockUnlock(1);

    while (1)
    {
    }

    return 0;
}