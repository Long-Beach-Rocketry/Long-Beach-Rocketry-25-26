#include "w25q.h"

/**
 * @brief Construct a new LBR::W25q::W25q object
 * 
 * @param spi_ 
 */
LBR::W25q::W25q(Spi& spi_, GpioChipSelect& cs_) : spi(spi_), cs(cs_)
{
}

/**
 * @brief Check BUSY bit in Status Reg-1
 * 
 * @return true W25Q is currently in a write or erase cycle
 * @return false W25Q is ready to accept commands
 */
bool LBR::W25q::BusyCheck()
{
    // Init tx and rx buf to send command and receive data
    uint8_t sr1_cmd = static_cast<uint8_t>(StatusRegister::STATUS_REGISTER_1);
    std::array<uint8_t, 1> sr1_val;

    // Chip select enable
    cs.ChipSelectEnable();

    // Send 0x05h command
    spi.Transfer(std::array<uint8_t, 1>{sr1_cmd}, sr1_val);

    // Chip select disable
    cs.ChipSelectDisable();

    // Check if BUSY bit is 1
    return (sr1_val[0] & 0x01) != 0;
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
    // Check BUSY bit for any current erase or writes
    while (BusyCheck())
    {
    }

    // Chip Select Enable

    // Send Status Read Command from status_reg_num

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
    // Check BUSY bit for any current erase or writes
    while (BusyCheck())
    {
    }

    // Create command tx buf

    // Chip Select Enable

    // Send Enable Reset (66h) command

    // Send Reset Device (99h) command

    // Chip Select Disable

    // Add 30 microsecond delay using time
}

/**
 * @brief Reads data from a sector, page, or word
 * 
 * @param sector 
 * @param page 
 * @param offset 
 * @param rxbuf 
 * @return true 
 * @return false 
 */
bool LBR::W25q::Read(uint16_t sector, uint8_t page, uint8_t offset,
                     std::span<uint8_t> rxbuf)
{
    // Return false if sector, page, or offset is outside of the threshold

    // Calculate Address of where to start read

    // Separate Address into three bytes

    // Create tx buf of read command and address bytes

    // Check BUSY bit for current erase or write

    // Chip Select Enable

    // Send txbuf

    // SPI read

    // Chip Select Disable

    return true;
}

/**
 * @brief Writes data to a page (256 bytes) and verifies the correct data was written
 * 
 * @param sector 4096 possible sectors from (0 - 4095)
 * @param page 16 possible pages in a sector (0 - 15)
 * @param offset 256 possible words in a page (0 - 255) ***Each word is a byte for the w25q***
 * @param txbuf 
 * @param rxbuf 
 * @return true 
 * @return false 
 */
bool LBR::W25q::PageProgram(uint16_t sector, uint8_t page, uint8_t offset,
                            std::span<uint8_t> txbuf, std::span<uint8_t> rxbuf)
{

    // Return false if sector, page, or offset is outside of the threshold

    // Calculate address of where to start write

    // Separate Address into three bytes

    // Combine write enable, page program instruction, calculated addr, and data into a txbuf to send

    // Check BUSY bit for current erase or write

    // Chip Select Enable

    // SPI Write txbuf
    spi.Write(std::span<const uint8_t>(buf.data(), tx_len));

    // Chip Select Disable

    // W25Q read

    return true;
}

/**
 * @brief Erase a sector
 * 
 * @param sector 
 * @return true 
 * @return false 
 */
bool LBR::W25q::SectorErase(uint16_t sector)
{
    // Return false if sector outside of threshold

    // Calculate address of where to start erase

    // Combine write enable, sector erase instruction and 24 bit address in tx buffer

    // Check BUSY bit for current erase or write
    while (BusyCheck())
    {
    }

    // Chip Select Enable
    cs.ChipSelectEnable();

    // SPI Write the txbuf

    // Chip Select Disable
    cs.ChipSelectDisable();

    // Check BUSY bit for any current erase or writes
    while (BusyCheck())
    {
    }

    // WEL bit check
    std::array<uint8_t, 1> rxbuf;
    do
    {
        StatusRead(StatusRegister::STATUS_REGISTER_1, rxbuf);
    } while (rxbuf[0] & 0x02);

    return true;
}

/**
 * @brief Erase entire chip
 * 
 * @return true 
 * @return false 
 */
bool LBR::W25q::ChipErase()
{
    // Check BUSY bit for any current erase or writes
    while (BusyCheck())
    {
    }

    return true;
}

/**
 * @brief Make a sector read only to prevent an accidental erase
 * 
 * @param sector 
 * @return true 
 * @return false 
 */
bool LBR::W25q::SectorProtect(uint16_t sector)
{
    return true;
}

/**
 * @brief Make a block read only to prevent an accidental erase
 * 
 * @param block 
 * @return true 
 * @return false 
 */
bool LBR::W25q::BlockProtect(uint8_t block)
{
    return true;
}