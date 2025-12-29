/**
 * @file w25q.h
 * @author Kent Hong
 * @brief API for the W25Q flash chip communicating via SPI
 * 
 */

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
    void W25qInit();
    void StatusRegWrite(StatusWrite status_reg_num, uint8_t status_byte);
    void StatusRegRead(StatusRead status_reg_num, std::span<uint8_t> rxbuf);
    void Reset();
    bool Read(uint16_t sector, uint8_t page, uint8_t offset, std::span<uint8_t> rxbuf);
    bool PageProgram(uint16_t sector, uint8_t page, uint8_t offset, std::span<uint8_t> txbuf, std::span<uint8_t> rxbuf);
    bool SectorErase(uint16_t sector);
    bool ChipErase();

    /**
    * @brief Make a block read only to prevent an accidental erase
    * 
    * @param block 
    * @return true Block successfully locked, false Block already locked
    */
    bool BlockLock(uint8_t block);

    /**
    * @brief Make a block writeable from read only mode
    * 
    * @param block 
    * @return true Block was successfully unlocked, false Block already unlocked
    */
    bool BlockUnlock(uint8_t block);

    // Bit Mask for a helper function
    static constexpr uint8_t kBlockBitMask = (1u << 0);

private:
    /**
    * @brief Check BUSY bit in Status Reg-1
    * 
    * @return true W25Q is currently in a write or erase cycle, false W25Q is ready to accept commands
    */
    bool BusyCheck();
    Spi& spi;
    GpioChipSelect& cs;

    // W25Q Opcodes from Instruction Set Table 1 in the datasheet
    struct Opcode
    {
        static constexpr uint8_t kGlobalBlockUnlock = 0x98u;
        static constexpr uint8_t kWriteEnable = 0x06u;
        static constexpr uint8_t kVolatileWriteEnable = 0x50u;
        static constexpr uint8_t kEnableReset = 0x66u;
        static constexpr uint8_t kResetDevice = 0x99u;
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