#include <cstdint>
#include "spi.h"


namespace LBR
{

enum class StatusRegister : uint8_t {
    STATUS_REGISTER_1 = 0x05,
    STATUS_REGISTER_2 = 0x35,
    STATUS_REGISTER_3 = 0x15
};

class W25q
{
public:
    W25q(Spi& spi_);

    bool StatusRead(StatusRegister status_reg_num, std::span<uint8_t> rxbuf);
    void Reset();
    bool Read(uint16_t sector, uint8_t page, std::span<uint8_t> rxbuf);
    bool PageProgram(uint16_t sector, uint8_t page, std::span<uint8_t> txbuf);
    bool SectorErase(uint16_t sector);
    bool ChipErase();

    // In case we want to erase entire chip except some blocks
    bool SectorProtect(uint16_t sector);
    bool BlockProtect(uint8_t block);

private:
    bool BusyCheck();
    Spi& spi;
};
}  // namespace LBR