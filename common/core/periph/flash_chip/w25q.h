/**
 * @file w25q.h
 * @author Kent Hong
 * @brief API for the W25Q flash chip communicating via SPI
 * 
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <span>
#include "gpio_cs.h"
#include "spi.h"

namespace LBR
{

class W25q
{
public:
    /**
     * @brief StatusWrite and StatusRead hold hex commands that indicate which 
     *        register you want to read or write into.
     * 
     */
    enum class StatusWrite : uint8_t
    {
        STATUS_REGISTER_1 = 0x01u,
        STATUS_REGISTER_2 = 0x31u,
        STATUS_REGISTER_3 = 0x11u
    };

    enum class StatusRead : uint8_t
    {
        STATUS_REGISTER_1 = 0x05u,
        STATUS_REGISTER_2 = 0x35u,
        STATUS_REGISTER_3 = 0x15u
    };

    /**
    * @brief Construct a new LBR::W25q::W25q object
    * 
    * @param spi_ SPI instance
    * @param cs_ Chip Select instance
    */
    explicit W25q(Spi& spi_, GpioChipSelect& cs_);

    /**
    * @brief Enable individual block and sector locks on device startup
    * 
    * @return true W25Q Initialization was successful, false Initialization failed
    */
    bool init();

    /**
    * @brief Volatile write to modify Status Reg values (used for block or sector protect)
    * 
    * @param status_reg_num Which number Status Reg we want to write to
    * @param mask Which bit positions we want to modify in the Status Reg, Ex. bit pos 4:2 -> 00011100 -> (0x07 << 2)
    * @param val The actual bit value we want to write into the Status Reg, Ex. value 101 on 4:2 -> 00010100 -> (0x05 << 2)
    * @return true Correct value was written to the Status Reg, false Operation failed
    */
    bool status_reg_write(StatusWrite status_reg_num, uint8_t mask,
                          uint8_t val);

    /**
    * @brief Read specified status register
    * 
    * @param status_reg_num 
    * @param rxbuf 
    * @return true Read operation on Status Reg success, false Read operation failed
    */
    bool status_reg_read(StatusRead status_reg_num, std::span<uint8_t> rxbuf);

    /**
    * @brief All on-going operations will be halted, the device will return to the default power-on state, and lose all current volatile settings 
    * 
    * @return true Device was successfully reset, false Device reset failed
    */
    bool reset();

    /**
    * @brief Reads data from a sector, page, or word
    * 
    * @param block
    * @param sector 
    * @param page 
    * @param offset 
    * @param rxbuf 
    * @return true Read from desired memory location successful, false Read failed
    */
    bool read(uint8_t block, uint8_t sector, uint8_t page, uint8_t offset,
              std::span<uint8_t> rxbuf);

    /**
    * @brief Writes data to a page (256 bytes) and verifies the correct data was written
    * 
    * @param block 256 possible blocks from (0 - 255)
    * @param sector 16 possible sectors per block from (0 - 15)
    * @param page 16 possible pages in a sector (0 - 15)
    * @param offset 256 possible words in a page (0 - 255) ***Each word is a byte for the w25q***
    * @param txbuf Data you want written into the flash chip
    * @param rxbuf Buffer to verify correct data was written into flash chip
    * @return true Data successfully written to desired memory location, false Write to flash chip failed
    */
    bool page_program(uint8_t block, uint8_t sector, uint8_t page,
                      uint8_t offset, std::span<uint8_t> txbuf,
                      std::span<uint8_t> rxbuf);

    /**
    * @brief Erase a 64KB block
    * 
    * @param block 
    * @return true Desired 64KB block successfully erased, false Block erase failed
    */
    bool block_erase(uint8_t block);

    /**
    * @brief Erase a sector
    * 
    * @param sector 
    * @return true Desired Sector successfully erased, false Sector was not erased
    */
    bool sector_erase(uint8_t block, uint8_t sector);

    /**
    * @brief Erase entire chip
    * 
    * @return true Chip successfully wiped, false Chip erase failed
    */
    bool chip_erase();

    /**
    * @brief Make a block read only to prevent an accidental erase
    * 
    * @param block 
    * @return true Block successfully locked, false Block already locked
    */
    bool block_lock(uint8_t block);

    /**
    * @brief Make a block writeable from read only mode
    * 
    * @param block 
    * @return true Block was successfully unlocked, false Block already unlocked
    */
    bool block_unlock(uint8_t block);

    // Bit Mask for a helper function
    static constexpr uint8_t kBlockBitMask = (1u << 0);

private:
    /**
    * @brief Check BUSY bit in Status Reg-1
    * 
    * @return true W25Q is currently in a write or erase cycle, false W25Q is ready to accept commands
    */
    bool busy_check();

    /**
    * @brief Non-volatile write enable
    * 
    * @return true WEL bit was set, false WEL bit is cleared
    */
    bool write_enable();

    /**
     * @brief Volatile write enable for writing to Status Registers
     * 
     * @return true Volatile Write Enable command was sent successfully, false otherwise
     */
    bool volatile_write_enable();

    /**
    * @brief Check if an individual block is locked or not
    * 
    * @param block_addr The address of the block that needs to be checked 
    * @param wps_val A byte that will hold the contents of whether the Block is locked or not
    * @return true Block is currently locked, false Block is not locked
    */
    bool block_lock_status_read(uint32_t block_addr, uint8_t& block_lock_byte);

    // Member Variables
    Spi& spi;
    GpioChipSelect& cs;

    // W25Q Opcodes from Instruction Set Table 1 in the datasheet
    struct Opcode
    {
        static constexpr uint8_t kGlobalBlockUnlock = 0x98u;
        static constexpr uint8_t kWriteEnable = 0x06u;
        static constexpr uint8_t kVolatileWriteEnable = 0x50u;
        static constexpr uint8_t kEnablereset = 0x66u;
        static constexpr uint8_t kresetDevice = 0x99u;
        static constexpr uint8_t kReadData = 0x03u;
        static constexpr uint8_t kPageProgram = 0x02u;
        static constexpr uint8_t kBlockErase64Kb = 0xD8u;
        static constexpr uint8_t kSectorErase = 0x20u;
        static constexpr uint8_t kChipErase = 0xC7u;
        static constexpr uint8_t kIndividualBlockLock = 0x36u;
        static constexpr uint8_t kIndividualBlockUnlock = 0x39u;
        static constexpr uint8_t kReadBlockLock = 0x3Du;
    };

    // Bit Masks
    static constexpr uint8_t kWpsMask = (1u << 2);
    static constexpr uint8_t kBusyMask = (1u << 0);
    static constexpr uint8_t kWelMask = (1u << 1);

    // Memory Sizes
    static constexpr uint32_t kBlockSizeBytes = 65536u;
    static constexpr uint32_t kSectorSizeBytes = 4096u;
    static constexpr uint32_t kPageSizeBytes = 256u;
    static constexpr uint32_t kOffsetSizeBit = 1u;

    //
};
}  // namespace LBR