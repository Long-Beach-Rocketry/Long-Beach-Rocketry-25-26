#include "w25q.h"

namespace LBR
{

W25q::W25q(Spi& spi_, GpioChipSelect& cs_) : spi{spi_}, cs{cs_}
{
}

bool W25q::W25qInit()
{
    // Read status reg to get current bits
    std::array<uint8_t, 1> status_reg_3;
    if (!this->StatusRegRead(StatusRead::STATUS_REGISTER_3, status_reg_3))
    {
        return false;
    }

    // Set WPS to enable individual block and sector lock
    if (!this->StatusRegWrite(StatusWrite::STATUS_REGISTER_3, (kWpsMask),
                              (kWpsMask)))
    {
        return false;
    }

    // By default on startup, all block lock bits are set to 1. We have to unlock all the blocks to write into them.
    if (!this->WriteEnable())
    {
        return false;
    }
    cs.ChipSelectEnable();
    std::array<uint8_t, 1> global_unlock_cmd{Opcode::kGlobalBlockUnlock};
    bool status = spi.Write(global_unlock_cmd);
    cs.ChipSelectDisable();

    return status;
}

bool W25q::BusyCheck()
{
    // Init tx and rx buf to send command and receive data
    uint8_t sr1_cmd = static_cast<uint8_t>(StatusRead::STATUS_REGISTER_1);
    std::array<uint8_t, 1> sr1_val;

    // Chip select enable
    cs.ChipSelectEnable();

    // Send 0x05h command
    std::array<uint8_t, 1> sr1_cmd_buf = {sr1_cmd};
    bool status = spi.SeqTransfer(sr1_cmd_buf, sr1_val);

    // Chip select disable
    cs.ChipSelectDisable();

    // Check if Sequential Transfer failed
    if (!status)
    {
        return false;
    }

    // Check if BUSY bit is 1
    return sr1_val[0] & kBusyMask;
}

bool W25q::StatusRegWrite(StatusWrite status_reg_num, uint8_t mask, uint8_t val)
{
    // Check for current writes or erases
    while (this->BusyCheck())
    {
    }

    // Get current value in desired status reg
    std::array<uint8_t, 1> status_reg_val;
    StatusRead status_read_cmd;
    if (status_reg_num == StatusWrite::STATUS_REGISTER_1)
    {
        status_read_cmd = StatusRead::STATUS_REGISTER_1;
    }
    else if (status_reg_num == StatusWrite::STATUS_REGISTER_2)
    {
        status_read_cmd = StatusRead::STATUS_REGISTER_2;
    }
    else
    {
        status_read_cmd = StatusRead::STATUS_REGISTER_3;
    }
    if (!this->StatusRegRead(status_read_cmd, status_reg_val))
    {
        return false;
    }

    // Create new byte to send to the status reg
    uint8_t new_byte = (status_reg_val[0] & ~mask) | (val & mask);

    // Enable Volatile Write
    if (!this->VolatileWriteEnable())
    {
        return false;
    }

    // Write a byte of data to desired status reg
    std::array<uint8_t, 2> txbuf = {static_cast<uint8_t>(status_reg_num),
                                    new_byte};
    cs.ChipSelectEnable();
    bool status = spi.Write(txbuf);
    cs.ChipSelectDisable();

    // Check if SPI write failed
    if (!status)
    {
        return false;
    }

    // Add delay of tw

    // Check busy bit
    while (this->BusyCheck())
    {
    }

    // Wait for write enable bit to clear
    std::array<uint8_t, 1> rxbuf;
    do
    {
        if (!StatusRegRead(StatusRead::STATUS_REGISTER_1, rxbuf))
        {
            return false;
        }
    } while (rxbuf[0] & kWelMask);

    // Check if correct value was written into the Status Reg
    if (!this->StatusRegRead(status_read_cmd, status_reg_val))
    {
        return false;
    }
    return (status_reg_val[0] & mask) == (val & mask);
}

bool W25q::StatusRegRead(StatusRead status_reg_num, std::span<uint8_t> rxbuf)
{
    // Chip Select Enable

    // Create tx buf of status reg number to read out of
    std::array<uint8_t, 1> status_reg_cmd = {
        static_cast<uint8_t>(status_reg_num)};

    // Send Status Read Command from status_reg_num

    // SPI Read

    // Chip Select Disable

    return status;
}

bool LBR::W25q::Reset()
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

    return status;
}

/**
 * @brief Writes data to a page (256 bytes) and verifies the correct data was written
 * 
 * @param block 256 possible blocks from (0 - 255)
 * @param sector 16 possible sectors per block from (0 - 15)
 * @param page 16 possible pages in a sector (0 - 15)
 * @param offset 256 possible words in a page (0 - 255) ***Each word is a byte for the w25q***
 * @param txbuf Data you want written into the flash chip
 * @param rxbuf Buffer to verify correct data was written into flash chip
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
    bool status = spi.Write(std::span<uint8_t>(buf.data(), tx_len));

    // Chip Select Disable

    // W25Q read

    return true;
}

bool W25q::BlockErase(uint8_t block)
{
    // Return false if block outside of threshold
    if (block > 255)
    {
        return false;
    }

    // Calculate address of where to start erase
    uint32_t addr = static_cast<uint32_t>(block) * kBlockSizeBytes;

    // Combine block erase instruction and 24 bit address in tx buffer
    std::array<uint8_t, 4> txbuf{
        Opcode::kBlockErase64Kb, static_cast<uint8_t>(addr >> 16),
        static_cast<uint8_t>(addr >> 8), static_cast<uint8_t>(addr)};

    // Check BUSY bit for current erase or write
    while (this->BusyCheck())
    {
    }

    // Erase the 64KB block
    if (!this->WriteEnable())
    {
        return false;
    }
    cs.ChipSelectEnable();
    bool status = spi.Write(txbuf);
    cs.ChipSelectDisable();

    // Check if SPI Write failed
    if (!status)
    {
        return false;
    }

    // Check BUSY bit for any current erase or writes
    while (this->BusyCheck())
    {
    }

    // Wait for write enable bit to clear
    std::array<uint8_t, 1> rxbuf;
    do
    {
        if (!StatusRegRead(StatusRead::STATUS_REGISTER_1, rxbuf))
        {
            return false;
        }
    } while (rxbuf[0] & kWelMask);

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
    uint32_t addr = static_cast<uint32_t>(block) * kBlockSizeBytes;
    addr += (static_cast<uint32_t>(sector) * kSectorSizeBytes);

    // Combine write enable, sector erase instruction and 24 bit address in tx buffer

    // Check BUSY bit for current erase or write
    while (this->BusyCheck())
    {
    }

    // Chip Select Enable
    cs.ChipSelectEnable();

    // Send cmd and addr to flash chip
    bool status = spi.Write(txbuf);

    // Chip Select Disable
    cs.ChipSelectDisable();

    // Check if SPI Write failed
    if (!status)
    {
        return false;
    }

    // Check BUSY bit for any current erase or writes
    while (this->BusyCheck())
    {
    }

    // Wait for write enable bit to clear
    std::array<uint8_t, 1> rxbuf;
    do
    {
        if (!StatusRegRead(StatusRead::STATUS_REGISTER_1, rxbuf))
        {
            return false;
        }
    } while (rxbuf[0] & kWelMask);

    return true;
}

bool W25q::ChipErase()
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
 * @brief Helper function for BlockLockStatusRead()
 * 
 * @param block_lock_byte 
 * @return true Block is locked, false Block is unlocked
 */
static inline bool IsBlockLocked(uint8_t block_lock_byte)
{
    return block_lock_byte & LBR::W25q::kBlockBitMask;
}

bool W25q::BlockLock(uint8_t block)
{
    // Calculate address of which block to check
    uint32_t addr = static_cast<uint32_t>(block) * kBlockSizeBytes;

    // Check if Block is already locked
    uint8_t block_lock_byte;
    if (!(this->BlockLockStatusRead(addr, block_lock_byte)))
    {
        return false;
    }
    if (IsBlockLocked(block_lock_byte))
    {
        return false;
    }

    // Lock the Block
    if (!this->WriteEnable())
    {
        return false;
    }
    std::array<uint8_t, 4> txbuf{
        Opcode::kIndividualBlockLock, static_cast<uint8_t>(addr >> 16),
        static_cast<uint8_t>(addr >> 8), static_cast<uint8_t>(addr)};
    cs.ChipSelectEnable();
    bool status = spi.Write(txbuf);
    cs.ChipSelectDisable();

    return status;
}

bool W25q::BlockUnlock(uint8_t block)
{
    // Calculate address of which block to check
    uint32_t addr = static_cast<uint32_t>(block) * kBlockSizeBytes;

    // Check if block is already unlocked
    uint8_t block_lock_byte;
    if (!(this->BlockLockStatusRead(addr, block_lock_byte)))
    {
        return false;
    }
    if (!IsBlockLocked(block_lock_byte))
    {
        return false;
    }

    // Unlock the Block
    if (!this->WriteEnable())
    {
        return false;
    }
    std::array<uint8_t, 4> txbuf{
        Opcode::kIndividualBlockUnlock, static_cast<uint8_t>(addr >> 16),
        static_cast<uint8_t>(addr >> 8), static_cast<uint8_t>(addr)};
    cs.ChipSelectEnable();
    bool status = spi.Write(txbuf);
    cs.ChipSelectDisable();

    return status;
}

bool W25q::BlockLockStatusRead(uint32_t block_addr, uint8_t& block_lock_byte)
{
    // Wait for current writes or erases to finish
    while (this->BusyCheck())
    {
    }

    // Send Block address and Block Lock Read cmd
    std::array<uint8_t, 4> txbuf{Opcode::kReadBlockLock,
                                 static_cast<uint8_t>(block_addr >> 16),
                                 static_cast<uint8_t>(block_addr >> 8),
                                 static_cast<uint8_t>(block_addr)};
    std::array<uint8_t, 1> block_lock_status;
    cs.ChipSelectEnable();
    bool status = spi.SeqTransfer(txbuf, block_lock_status);
    cs.ChipSelectDisable();

    block_lock_byte = block_lock_status[0];

    return status;
}
}  // namespace LBR
