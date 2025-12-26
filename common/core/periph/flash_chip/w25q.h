#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <span>
#include "gpio_cs.h"

#include "spi.h"

namespace LBR
{

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

class W25q
{
public:
    W25q(Spi& spi_, GpioChipSelect& cs_);

    void W25qInit();
    bool StatusRegWrite(StatusWrite status_reg_num, uint8_t status_byte);
    bool StatusRegRead(StatusRead status_reg_num, std::span<uint8_t> rxbuf);
    void Reset();
    bool Read(uint16_t sector, uint8_t page, uint8_t offset, std::span<uint8_t> rxbuf);
    bool PageProgram(uint16_t sector, uint8_t page, uint8_t offset, std::span<uint8_t> txbuf, std::span<uint8_t> rxbuf);
    bool SectorErase(uint16_t sector);
    bool ChipErase();

    // In case we want to erase entire chip except some blocks
    bool BlockLock(uint8_t block);
    bool BlockUnlock(uint8_t block);

private:
    bool BusyCheck();
    Spi& spi;
    GpioChipSelect& cs;
};
}  // namespace LBR