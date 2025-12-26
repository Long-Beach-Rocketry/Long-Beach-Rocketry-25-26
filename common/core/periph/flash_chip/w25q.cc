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
 * @brief Enable individual block and sector locks on device startup
 * 
 * @return void
 */
void LBR::W25q::W25qInit()
{
    // Read status reg to get current bits
    std::array<uint8_t, 1> status_reg_3;
    this->StatusRegRead(StatusRead::STATUS_REGISTER_3, status_reg_3);

    // Make new byte to write into status reg 3 to enable WPS bit
    status_reg_3[0] |= 0x04u;

    // Set WPS to enable individual block and sector lock
    this->StatusRegWrite(StatusWrite::STATUS_REGISTER_3, status_reg_3[0]);

    // By default on startup, all block lock bits are set to 1. We have to unlock all the blocks to write into them.
    this->WriteEnable();
    cs.ChipSelectEnable();
    std::array<uint8_t, 1> global_unlock_cmd{0x98u};
    spi.Write(global_unlock_cmd);
    cs.ChipSelectDisable();
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
    uint8_t sr1_cmd = static_cast<uint8_t>(StatusRead::STATUS_REGISTER_1);
    std::array<uint8_t, 1> sr1_val;

    // Chip select enable
    cs.ChipSelectEnable();

    // Send 0x05h command
    std::array<uint8_t, 1> sr1_cmd_buf = {sr1_cmd};
    spi.Transfer(sr1_cmd_buf, sr1_val);

    // Chip select disable
    cs.ChipSelectDisable();

    // Check if BUSY bit is 1
    return (sr1_val[0] & 0x01) != 0;
}

/**
 * @brief Modify Status Reg values (used for block or sector protect)
 * 
 * @param status_reg_num 
 * @return true 
 * @return false 
 */
bool LBR::W25q::StatusRegWrite(StatusWrite status_reg_num, uint8_t status_byte)
{
    // Check for current writes or erases
    while (this->BusyCheck())
    {
    }

    // Enable Write
    this->WriteEnable();

    // Chip Select Enable
    cs.ChipSelectEnable();

    // Create tx buf of status reg number and the byte of data to write into
    std::array<uint8_t, 2> txbuf = {static_cast<uint8_t>(status_reg_num),
                                    status_byte};

    // Write a byte of data to desired status reg
    spi.Write(txbuf);

    cs.ChipSelectDisable();

    // Add delay of tw

    // Check busy bit
    while (this->BusyCheck())
    {
    }

    // Wait for write enable bit to clear
    std::array<uint8_t, 1> rxbuf;
    do
    {
        StatusRegRead(StatusRead::STATUS_REGISTER_1, rxbuf);
    } while (rxbuf[0] & 0x02);

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
bool LBR::W25q::StatusRegRead(StatusRead status_reg_num,
                              std::span<uint8_t> rxbuf)
{
    // Check BUSY bit for any current erase or writes
    while (this->BusyCheck())
    {
    }

    // Chip Select Enable

    // Create tx buf of status reg number to read out of
    std::array<uint8_t, 1> status_reg_cmd = {
        static_cast<uint8_t>(status_reg_num)};

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
    while (this->BusyCheck())
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
    spi.Write(std::span<uint8_t>(buf.data(), tx_len));

    // Chip Select Disable

    // W25Q read

    return true;
}

/**
 * @brief Erase a 64KB block
 * 
 * @param block 
 * @return true 
 * @return false 
 */
bool LBR::W25q::BlockErase(uint8_t block)
{
    // Return false if block outside of threshold
    if (block > 255)
    {
        return false;
    }

    // Calculate address of where to start erase
    uint32_t addr = 0x00;
    addr += (static_cast<uint32_t>(block) * 0x010000u);

    // Combine block erase instruction and 24 bit address in tx buffer
    std::array<uint8_t, 4> txbuf{0xD8u, static_cast<uint8_t>(addr >> 16),
                                 static_cast<uint8_t>(addr >> 8),
                                 static_cast<uint8_t>(addr)};

    // Check BUSY bit for current erase or write
    while (this->BusyCheck())
    {
    }

    // Erase the 64KB block
    this->WriteEnable();
    cs.ChipSelectEnable();
    spi.Write(txbuf);
    cs.ChipSelectDisable();

    // Check BUSY bit for any current erase or writes
    while (this->BusyCheck())
    {
    }

    // Wait for write enable bit to clear
    std::array<uint8_t, 1> rxbuf;
    do
    {
        StatusRegRead(StatusRead::STATUS_REGISTER_1, rxbuf);
    } while (rxbuf[0] & 0x02);

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
    // Return false if block or sector outside of threshold
    if (block > 255 || sector > 15)
    {
        return false;
    }

    // Calculate address of where to start erase
    uint32_t addr = 0x00;
    addr += (static_cast<uint32_t>(block) * 0x010000u);
    addr += (static_cast<uint32_t>(sector) * 0x001000u);

    // Combine write enable, sector erase instruction and 24 bit address in tx buffer

    // Check BUSY bit for current erase or write
    while (this->BusyCheck())
    {
    }

    // Chip Select Enable
    cs.ChipSelectEnable();

    // Send cmd and addr to flash chip
    spi.Write(txbuf);

    // Chip Select Disable
    cs.ChipSelectDisable();

    // Check BUSY bit for any current erase or writes
    while (this->BusyCheck())
    {
    }

    // Wait for write enable bit to clear
    std::array<uint8_t, 1> rxbuf;
    do
    {
        StatusRegRead(StatusRead::STATUS_REGISTER_1, rxbuf);
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
    while (this->BusyCheck())
    {
    }

    return true;
}

    return true;
}

/**
 * @brief Make a block read only to prevent an accidental erase
 * 
 * @param block 
 * @return true Block successfully locked
 * @return false Block is already locked
 */
bool LBR::W25q::BlockLock(uint8_t block)
{
    std::array<uint8_t, 1> block_lock_status;

    // Calculate address of which block to check
    uint32_t addr = 0x00;
    addr += (static_cast<uint32_t>(block) * 0x010000u);

    // Check if Block is already locked
    if (this->BlockLockStatusRead(addr, block_lock_status))
    {
        return false;
    }

    // Lock the Block
    this->WriteEnable();
    std::array<uint8_t, 4> txbuf{0x36u, static_cast<uint8_t>(addr >> 16),
                                 static_cast<uint8_t>(addr >> 8),
                                 static_cast<uint8_t>(addr)};
    cs.ChipSelectEnable();
    spi.Write(txbuf);
    cs.ChipSelectDisable();

    return true;
}

/**
 * @brief Make a block writeable from read only mode
 * 
 * @param block 
 * @return true Block was successfully unlocked
 * @return false Block was already unlocked
 */
bool LBR::W25q::BlockUnlock(uint8_t block)
{
    std::array<uint8_t, 1> block_lock_status;

    // Calculate address of which block to check
    uint32_t addr = 0x00;
    addr += (static_cast<uint32_t>(block) * 0x010000u);

    if (!(this->BlockLockStatusRead(block, block_lock_status)))
    {
        return false;
    }

    // Unlock the Block
    this->WriteEnable();
    std::array<uint8_t, 4> txbuf{0x39u, static_cast<uint8_t>(addr >> 16),
                                 static_cast<uint8_t>(addr >> 8),
                                 static_cast<uint8_t>(addr)};
    cs.ChipSelectEnable();
    spi.Write(txbuf);
    cs.ChipSelectDisable();

    return true;
}

/**
 * @brief Check if an individual block is locked or not
 * 
 * @param block 
 * @param block_lock_status 
 * @return true Block is currently locked
 * @return false Block is not locked
 */
bool LBR::W25q::BlockLockStatusRead(uint32_t block_addr,
                                    std::array<uint8_t, 1> block_lock_status)
{
    // Wait for current writes or erases to finish
    while (this->BusyCheck())
    {
    }

    cs.ChipSelectEnable();
    std::array<uint8_t, 4> txbuf{0x3Du, static_cast<uint8_t>(block_addr >> 16),
                                 static_cast<uint8_t>(block_addr >> 8),
                                 static_cast<uint8_t>(block_addr)};
    spi.Transfer(txbuf, block_lock_status);
    cs.ChipSelectDisable();

    return static_cast<bool>(block_lock_status[0] & 0x01u);
}