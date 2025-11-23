#include "w25q.h"

/**
 * @brief Construct a new LBR::W25q::W25q object
 * 
 * @param spi_ 
 */
LBR::W25q::W25q(Spi& spi_) : spi(spi_)
{
}

/**
 * @brief Check BUSY status in status reg 1
 * 
 * @return true 
 * @return false 
 */
bool LBR::W25q::BusyCheck()
{

    return true;
}

/**
 * @brief Read specified status register
 * 
 * @param status_reg_num 
 * @param rxbuf 
 * @return true 
 * @return false 
 */
bool LBR::W25q::StatusRead(StatusRegister status_reg_num,
                           std::span<uint8_t> rxbuf)
{
    // Chip Select Enable

    // Check BUSY bit for any current erase or writes

    // Chip Select Disable

    // Chip Select Enable

    // Send Status Read Command from status_reg_num

    // Chip Select Disable

    // Chip Select Enable

    // SPI Read

    // Chip Select Disable

    return true;
}

/**
 * @brief All on-going operations will be halted, the device will return to the default power-on state, and lose all current volatile settings 
 * 
 * @return true 
 * @return false 
 */
void LBR::W25q::Reset()
{
    // Chip Select Enable

    // Check BUSY bit for any current erase or writes

    // Chip Select Disable

    // Chip Select Enable

    // Send Enable Reset (66h) command

    // Chip Select Disable

    // Chip Select Enable

    // Send Reset Device (99h) command

    // Chip Select Disable

    // Add 30 microsecond delay using time
}

bool LBR::W25q::Read(uint16_t sector, uint8_t page, std::span<uint8_t> rxbuf)
{
    // Calculate Address

    // Separate Address into three bytes

    // Chip Select Enable

    // Check BUSY bit for current erase or write

    // Send Read command (03h)

    // Send 24-bit address

    // SPI read

    // Chip Select Disable

    return true;
}