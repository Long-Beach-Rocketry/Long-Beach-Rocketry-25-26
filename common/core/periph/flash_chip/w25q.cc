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
    cs.ChipSelectEnable();

    // Create tx buf of status reg number to read out of
    std::array<uint8_t, 1> status_reg_cmd = {
        static_cast<uint8_t>(status_reg_num)};

    // Send Status Read Command from status_reg_num
    bool status = spi.SeqTransfer(status_reg_cmd, rxbuf);

    // Chip Select Disable
    cs.ChipSelectDisable();

    return status;
}

bool W25q::WriteEnable()
{
    // Check BUSY bit for any current erase or writes
    while (this->BusyCheck())
    {
    }

    // Chip Select Enable
    cs.ChipSelectEnable();

    // Create Write Enable Command tx buf
    std::array<uint8_t, 1> write_en_cmd = {Opcode::kWriteEnable};

    // Write Enable instruction 06h
    bool status = spi.Write(write_en_cmd);

    // Chip Select Disable
    cs.ChipSelectDisable();

    // Check if SPI Write failed
    if (!status)
    {
        return false;
    }

    // Check if WEL bit was set
    std::array<uint8_t, 1> status_reg_val;
    if (!this->StatusRegRead(StatusRead::STATUS_REGISTER_1, status_reg_val))
    {
        return false;
    }
    return status_reg_val[0] & kWelMask;
}

bool W25q::VolatileWriteEnable()
{
    // Check BUSY bit for any ongoing erase or writes
    while (this->BusyCheck())
    {
    }

    // Send Volatile Write Enable cmd
    std::array<uint8_t, 1> volatile_write_en{Opcode::kVolatileWriteEnable};
    cs.ChipSelectEnable();
    bool status = spi.Write(volatile_write_en);
    cs.ChipSelectDisable();

    return status;
}

bool W25q::Reset()
{
    // Check BUSY bit for any current erase or writes
    while (this->BusyCheck())
    {
    }

    // Set WEL bit to check afterwards if Reset was successful and WEL was cleared
    if (!this->WriteEnable())
    {
        return false;
    }

    // Enable Reset
    std::array<uint8_t, 1> enable_reset_cmd{Opcode::kEnableReset};
    cs.ChipSelectEnable();
    bool status = spi.Write(enable_reset_cmd);
    cs.ChipSelectDisable();
    if (!status)
    {
        return false;
    }

    // Reset Device
    std::array<uint8_t, 1> reset_cmd{Opcode::kResetDevice};
    cs.ChipSelectEnable();
    status = spi.Write(reset_cmd);
    cs.ChipSelectDisable();
    if (!status)
    {
        return false;
    }

    // Add 30 microsecond delay using timer

    // Check if WEL bit was cleared after reset
    std::array<uint8_t, 1> status_reg_val;
    if (!StatusRegRead(StatusRead::STATUS_REGISTER_1, status_reg_val))
    {
        return false;
    }
    return !(status_reg_val[0] & kWelMask);
}

bool W25q::Read(uint8_t block, uint8_t sector, uint8_t page, uint8_t offset,
                std::span<uint8_t> rxbuf)
{
    // Return false if sector or page is outside of the threshold
    if (sector > 15 || page > 15)
    {
        return false;
    }

    // Calculate 24 bit Address of where to start read
    uint32_t addr = static_cast<uint32_t>(block) * kBlockSizeBytes;
    addr += (static_cast<uint32_t>(sector) * kSectorSizeBytes);
    addr += (static_cast<uint32_t>(page) * kPageSizeBytes);
    addr += (static_cast<uint32_t>(offset) * kOffsetSizeBit);

    // Create tx buf of read command and address bytes
    std::array<uint8_t, 4> txbuf = {
        Opcode::kReadData, static_cast<uint8_t>(addr >> 16),
        static_cast<uint8_t>(addr >> 8), static_cast<uint8_t>(addr)};

    // Check BUSY bit for current erase or write
    while (this->BusyCheck())
    {
    }

    // Chip Select Enable
    cs.ChipSelectEnable();

    // Send txbuf and read from memory
    bool status = spi.SeqTransfer(txbuf, rxbuf);

    // Chip Select Disable
    cs.ChipSelectDisable();

    return status;
}

bool W25q::PageProgram(uint8_t block, uint8_t sector, uint8_t page,
                       uint8_t offset, std::span<uint8_t> txbuf,
                       std::span<uint8_t> rxbuf)
{

    // Return false if block, sector, page is outside of the threshold
    if (block > 255 || sector > 15 || page > 15 || offset > 255)
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
    uint32_t addr = static_cast<uint32_t>(block) * kBlockSizeBytes;
    addr += (static_cast<uint32_t>(sector) * kSectorSizeBytes);
    addr += (static_cast<uint32_t>(page) * kPageSizeBytes);
    addr += (static_cast<uint32_t>(offset) * kOffsetSizeBit);

    // Combine page program instruction, calculated addr, and data into a buf to send
    std::array<uint8_t, 260> buf{
        Opcode::kPageProgram, static_cast<uint8_t>(addr >> 16),
        static_cast<uint8_t>(addr >> 8), static_cast<uint8_t>(addr)};

    std::memcpy(&buf[4], txbuf.data(), txbuf.size());
    size_t tx_len = 4 + txbuf.size();

    // Check BUSY bit for current erase or write
    while (this->BusyCheck())
    {
    }

    // Write Enable
    if (!this->WriteEnable())
    {
        return false;
    }

    // Chip Select Enable
    cs.ChipSelectEnable();

    // SPI Write txbuf
    bool status = spi.Write(std::span<uint8_t>(buf.data(), tx_len));

    // Chip Select Disable
    cs.ChipSelectDisable();

    // Check if SPI Write failed
    if (!status)
    {
        return false;
    }

    // W25Q read to verify correct data was written
    if (!this->Read(block, sector, page, offset, rxbuf))
    {
        return false;
    }
    return std::equal(rxbuf.begin(), rxbuf.end(), txbuf.begin(), txbuf.end());
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

bool W25q::SectorErase(uint8_t block, uint8_t sector)
{
    // Return false if block or sector outside of threshold
    if (block > 255 || sector > 15)
    {
        return false;
    }

    // Calculate address of where to start erase
    uint32_t addr = static_cast<uint32_t>(block) * kBlockSizeBytes;
    addr += (static_cast<uint32_t>(sector) * kSectorSizeBytes);

    // Combine sector erase instruction and 24 bit address in tx buffer
    std::array<uint8_t, 4> txbuf{
        Opcode::kSectorErase, static_cast<uint8_t>(addr >> 16),
        static_cast<uint8_t>(addr >> 8), static_cast<uint8_t>(addr)};

    // Check BUSY bit for current erase or write
    while (this->BusyCheck())
    {
    }

    // Write Enable
    if (!this->WriteEnable())
    {
        return false;
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

    // Write Enable
    if (!this->WriteEnable())
    {
        return false;
    }

    // Send Chip Erase instruction C7h or 60h
    cs.ChipSelectEnable();
    std::array<uint8_t, 1> chip_erase_cmd = {Opcode::kChipErase};
    bool status = spi.Write(chip_erase_cmd);
    cs.ChipSelectDisable();

    // Check if SPI Write failed
    if (!status)
    {
        return false;
    }

    // Wait delay of tCE

    // Wait for Chip Erase to complete before ending
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