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
    cs.ChipSelectEnable();

    // Send Status Read Command from status_reg_num
    spi.Transfer(std::array<uint8_t, 1>{static_cast<uint8_t>(status_reg_num)},
                 rxbuf);

    // Chip Select Disable
    cs.ChipSelectDisable();

    return true;
}

void LBR::W25q::WriteEnable()
{
    // Check BUSY bit for any current erase or writes
    while (BusyCheck())
    {
    }

    // Chip Select Enable
    cs.ChipSelectEnable();

    // Write Enable instruction 06h
    spi.Write(std::array<uint8_t, 1>{0x06u});

    // Chip Select Disable
    cs.ChipSelectDisable();
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

    // Chip Select Enable
    cs.ChipSelectEnable();

    // Send Enable Reset (66h) command and Reset Device (99h) command
    spi.Write(std::array<uint8_t, 2>{0x66u, 0x99u});

    // Chip Select Disable
    cs.ChipSelectDisable();

    // Add 30 microsecond delay using timer
}

/**
 * @brief Reads data from a sector, page, or word
 * 
 * @param block
 * @param sector 
 * @param page 
 * @param offset 
 * @param rxbuf 
 * @return true 
 * @return false 
 */
bool LBR::W25q::Read(uint8_t block, uint8_t sector, uint8_t page,
                     uint8_t offset, std::span<uint8_t> rxbuf)
{
    // Return false if sector or page is outside of the threshold
    if (sector > 15 || page > 15)
    {
        return false;
    }

    // Calculate 24 bit Address of where to start read
    uint32_t addr = 0x00;
    addr += (static_cast<uint32_t>(block) * 0x010000u);
    addr += (static_cast<uint32_t>(sector) * 0x001000u);
    addr += (static_cast<uint32_t>(page) * 0x000100u);
    addr += (static_cast<uint32_t>(offset) * 0x000001u);

    // Create tx buf of read command and address bytes
    std::array<uint8_t, 4> txbuf = {0x03, static_cast<uint8_t>(addr >> 16),
                                    static_cast<uint8_t>(addr >> 8),
                                    static_cast<uint8_t>(addr)};

    // Check BUSY bit for current erase or write
    while (BusyCheck())
    {
    }

    // Chip Select Enable
    cs.ChipSelectEnable();

    // Send txbuf and read from memory
    spi.Transfer(txbuf, rxbuf);

    // Chip Select Disable
    cs.ChipSelectDisable();

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
bool LBR::W25q::PageProgram(uint8_t block, uint8_t sector, uint8_t page,
                            uint8_t offset, std::span<uint8_t> txbuf,
                            std::span<uint8_t> rxbuf)
{

    // Return false if sector, page, or offset is outside of the threshold
    if (sector > 15 || page > 15)
    {
        return false;
    }

    // Can only write up to 256 bytes at a time
    if (txbuf.size() > 256)
    {
        return false;
    }

    // Cannot overflow pages when writing
    if (offset + txbuf.size() > 256)
    {
        return false;
    }

    // Calculate 24 bit Address of where to start write
    uint32_t addr = 0x00;
    addr += (static_cast<uint32_t>(block) * 0x010000u);
    addr += (static_cast<uint32_t>(sector) * 0x001000u);
    addr += (static_cast<uint32_t>(page) * 0x000100u);
    addr += (static_cast<uint32_t>(offset) * 0x000001u);

    // Combine page program instruction, calculated addr, and data into a buf to send
    std::array<uint8_t, 260> buf{0x02u, static_cast<uint8_t>(addr >> 16),
                                 static_cast<uint8_t>(addr >> 8),
                                 static_cast<uint8_t>(addr), 0x00};

    std::memcpy(&buf[4], txbuf.data(), txbuf.size());
    size_t tx_len = 4 + txbuf.size();

    // Check BUSY bit for current erase or write
    while (BusyCheck())
    {
    }

    // Write Enable
    this->WriteEnable();

    // Chip Select Enable
    cs.ChipSelectEnable();

    // SPI Write txbuf
    spi.Write(std::span<const uint8_t>(buf.data(), tx_len));

    // Chip Select Disable
    cs.ChipSelectDisable();

    // W25Q read to verify correct data was written
    this->Read(block, sector, page, offset, rxbuf);
    return std::equal(rxbuf.begin(), rxbuf.end(), txbuf.begin(), txbuf.end());
}

/**
 * @brief Erase a sector
 * 
 * @param sector 
 * @return true 
 * @return false 
 */
bool LBR::W25q::SectorErase(uint8_t block, uint8_t sector)
{
    // Return false if sector outside of threshold

    // Calculate address of where to start erase

    // Combine sector erase instruction and 24 bit address in tx buffer

    // Check BUSY bit for current erase or write
    while (BusyCheck())
    {
    }

    // Write Enable
    this->WriteEnable();

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

    // Write Enable
    this->WriteEnable();

    // CS Low
    cs.ChipSelectEnable();

    // Send Chip Erase instruction C7h or 60h
    spi.Write(std::array<uint8_t, 1>{0xC7});

    // CS High
    cs.ChipSelectDisable();

    // Wait delay of tCE

    // Wait for Chip Erase to complete before ending
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