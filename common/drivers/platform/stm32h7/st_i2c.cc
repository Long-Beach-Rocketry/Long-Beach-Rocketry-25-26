#include "st_i2c.h"
// #include "delay.h"

// | 1 for read
// | 0 for write

namespace LBR
{
namespace Stmh7
{
constexpr uint8_t kShiftSADD{
    1};  // Shift slave address up 1 because w/r go to 0 position
constexpr uint8_t kNBytes8Bit{8 / 8};    // 8-bit register addr to bytes
constexpr uint8_t kNBytes16Bit{16 / 8};  // 16-bit register addr to bytes
constexpr uint8_t kRegAddrMask{0xFF};    // Mask for 8-bit register addresses
constexpr uint8_t k16BitHalf{16 / 2};

HwI2c::HwI2c(const StI2cParams& params)
    : base_addr{params.base_addr},
      timingr{params.timingr},
      timeout_delay_ms{params.timeout_delay_ms}
{
}

bool HwI2c::init()
{
    if (base_addr)
    {
        base_addr->CR1 &= ~I2C_CR1_PE;  // Reset the peripheral

        base_addr->TIMINGR = timingr;  // Set the timing register

        base_addr->CR2 &= ~I2C_CR2_ADD10;  // 7-bit addressing mode

        base_addr->CR1 |= I2C_CR1_PE;  // Enabled the peripheral

        return true;
    }
    return false;
}

/* Private Helpers */

bool HwI2c::is_enabled()
{
    if (base_addr)  // check if not nullptr
    {
        // Return if peripheral is enabled
        return (base_addr->CR1 & I2C_CR1_PE);
    }
    return false;
}

bool HwI2c::bus_free()
{
    if (is_enabled())
    {
        // Return if busy
        return !(base_addr->ISR & I2C_ISR_BUSY);
    }
    return false;
}

bool HwI2c::timed_out(uint32_t flag)
{

    uint32_t timeout = timeout_delay_ms;
    while (--timeout)
    {
        // LBR::Utils::DelayMs(1);
        // I2C Error flags
        if (base_addr->ISR & (I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO))
        {
            // Clear the error flags if raised
            base_addr->ICR |= I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF;
            return true;
        }

        // Flag we are waiting for has been raised!
        if (base_addr->ISR & flag)
        {
            return false;
        }
    }
    return true;
}

/* Public methods */

bool HwI2c::mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                     uint8_t dev_addr)
{
    /* Check if enabled & ready */
    if (!bus_free())
    {
        return false;
    }

    /* Write slave address, Write mode */
    base_addr->CR2 &= ~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk |
                        I2C_CR2_AUTOEND);

    // Shift SADD by 1+ (w/r indicator will go at 0)
    // NBYTES set as 1 (reg_addr size / 8)
    base_addr->CR2 |= ((dev_addr << (I2C_CR2_SADD_Pos + kShiftSADD)) |
                       I2C_CR2_START | (kNBytes8Bit << I2C_CR2_NBYTES_Pos));

    /* Transmit register address */
    if (timed_out(I2C_ISR_TXIS))
    {
        return false;
    }
    base_addr->TXDR = reg_addr;

    if (timed_out(I2C_ISR_TC))
    {
        return false;
    }

    /* Write slave address, Read mode */
    base_addr->CR2 &= ~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk |
                        I2C_CR2_AUTOEND);

    // Shift SADD by 1 (w/r indicator will go there)
    // Shift NBYTES by data size
    base_addr->CR2 |=
        ((dev_addr << (I2C_CR2_SADD_Pos + kShiftSADD)) | I2C_CR2_RD_WRN |
         I2C_CR2_START |
         (static_cast<uint32_t>(data.size() & 0xFF) << I2C_CR2_NBYTES_Pos) |
         I2C_CR2_AUTOEND);

    /* Read loop */
    for (uint8_t& byte : data)
    {
        if (timed_out(I2C_ISR_RXNE))
        {
            return false;
        }

        byte = base_addr->RXDR;
    }

    if (timed_out(I2C_ISR_STOPF))
    {
        return false;
    }

    base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                     uint8_t dev_addr)
{
    /* Check if enabled & ready */
    if (!bus_free())
    {
        return false;
    }

    /* Write slave address, Write mode */
    base_addr->CR2 &= ~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk |
                        I2C_CR2_AUTOEND);

    // Shift SADD by 1+ (w/r indicator will go at 0)
    // NBYTES set as 2 (reg_addr size / 8)
    base_addr->CR2 |= ((dev_addr << (kShiftSADD + I2C_CR2_SADD_Pos)) |
                       I2C_CR2_START | (kNBytes16Bit << I2C_CR2_NBYTES_Pos));

    /* Transmit both halves of the register address */
    if (timed_out(I2C_ISR_TXIS))
    {
        return false;
    }
    base_addr->TXDR = static_cast<uint8_t>((reg_addr >> k16BitHalf) &
                                           kRegAddrMask);  // Upper half

    if (timed_out(I2C_ISR_TXIS))
    {
        return false;
    }
    base_addr->TXDR =
        static_cast<uint8_t>(reg_addr & kRegAddrMask);  // Lower half

    if (timed_out(I2C_ISR_TC))
    {
        return false;
    }

    /* Write slave address, Read mode */
    base_addr->CR2 &= ~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk |
                        I2C_CR2_AUTOEND);
    base_addr->CR2 |=
        ((dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_RD_WRN | I2C_CR2_START |
         (static_cast<uint32_t>((data.size() + 1) & 0xFF)
          << I2C_CR2_NBYTES_Pos) |
         I2C_CR2_AUTOEND);

    /* Read loop */
    for (uint8_t& byte : data)
    {
        if (timed_out(I2C_ISR_RXNE))
        {
            return false;
        }

        byte = base_addr->RXDR;
    }
    if (timed_out(I2C_ISR_STOPF))
    {
        return false;
    }
    base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                      uint8_t dev_addr)
{
    /* Check if enabled & ready */
    if (!bus_free())
    {
        return false;
    }

    /* Write slave address, Write mode */
    base_addr->CR2 &= ~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk |
                        I2C_CR2_AUTOEND);

    // Shift SADD by 1+ (w/r indicator will go at 0)
    // NBYTES set as 1 (reg_addr size / 8) plus the data size
    base_addr->CR2 |=
        ((dev_addr << (kShiftSADD + I2C_CR2_SADD_Pos)) | I2C_CR2_START |
         (static_cast<uint32_t>((data.size() + kNBytes8Bit) & 0xFF)
          << I2C_CR2_NBYTES_Pos) |
         I2C_CR2_AUTOEND);

    if (timed_out(I2C_ISR_TXIS))
    {
        return false;
    }

    /* Transmit register address */
    base_addr->TXDR = reg_addr;

    /* Write loop */
    for (const uint8_t byte : data)
    {
        if (timed_out(I2C_ISR_TXIS))
        {
            return false;
        }
        if (base_addr->ISR & I2C_ISR_NACKF)
        {
            base_addr->ICR |= I2C_ICR_NACKCF;
            return false;
        }

        base_addr->TXDR = byte;
    }

    if (timed_out(I2C_ISR_STOPF))
    {
        return false;
    }
    base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                      uint8_t dev_addr)
{
    /* Check if enabled & ready */
    if (!bus_free())
    {
        return false;
    }

    /* Write slave address, Write mode */
    base_addr->CR2 &= ~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk |
                        I2C_CR2_AUTOEND);

    // Shift SADD by 1+ (w/r indicator will go at 0)
    // NBYTES set as 2 (reg_addr size / 8) plus data size
    base_addr->CR2 |=
        ((dev_addr << (kShiftSADD + I2C_CR2_SADD_Pos)) | I2C_CR2_START |
         (static_cast<uint32_t>((data.size() + kNBytes16Bit) & 0xFF)
          << I2C_CR2_NBYTES_Pos) |
         I2C_CR2_AUTOEND);

    /* Transmit both halves of the register address */
    if (timed_out(I2C_ISR_TXIS))
    {
        return false;
    }
    base_addr->TXDR = static_cast<uint8_t>((reg_addr >> k16BitHalf) &
                                           kRegAddrMask);  // Upper half

    if (timed_out(I2C_ISR_TXIS))
    {
        return false;
    }
    base_addr->TXDR =
        static_cast<uint8_t>(reg_addr & kRegAddrMask);  // Lower half

    /* Write loop */
    for (const uint8_t byte : data)
    {
        if (timed_out(I2C_ISR_TXIS))
        {
            return false;
        }
        if (base_addr->ISR & I2C_ISR_NACKF)
        {
            base_addr->ICR |= I2C_ICR_NACKCF;
            return false;
        }

        base_addr->TXDR = byte;
    }

    if (timed_out(I2C_ISR_STOPF))
    {
        return false;
    }
    base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::read(std::span<uint8_t> data, uint8_t dev_addr)
{
    /* Check if enabled & ready */
    if (!(bus_free()))
    {
        return false;
    }

    /* Write slave address, Read mode */
    base_addr->CR2 &= ~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk |
                        I2C_CR2_AUTOEND);
    base_addr->CR2 |=
        ((dev_addr << (I2C_CR2_SADD_Pos + kShiftSADD)) | I2C_CR2_RD_WRN |
         I2C_CR2_START |
         (static_cast<uint32_t>(data.size() & 0xFF) << I2C_CR2_NBYTES_Pos) |
         I2C_CR2_AUTOEND);

    /* Read loop */
    for (uint8_t& byte : data)
    {
        if (timed_out(I2C_ISR_RXNE))
        {
            return false;
        }

        byte = base_addr->RXDR;
    }

    if (timed_out(I2C_ISR_STOPF))
    {
        return false;
    }
    base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::write(std::span<const uint8_t> data, uint8_t dev_addr)
{
    /* Check if enabled & ready */
    if (!bus_free())
    {
        return false;
    }

    /* Write slave address, Write mode */
    base_addr->CR2 &= ~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk |
                        I2C_CR2_AUTOEND);
    base_addr->CR2 |=
        ((dev_addr << (kShiftSADD + I2C_CR2_SADD_Pos)) | I2C_CR2_START |
         (static_cast<uint32_t>(data.size() & 0xFF) << I2C_CR2_NBYTES_Pos) |
         I2C_CR2_AUTOEND);

    /* Write loop */
    for (const uint8_t byte : data)
    {

        if (timed_out(I2C_ISR_TXIS))
        {
            return false;
        }
        if (I2C_ISR_NACKF & base_addr->ISR)
        {
            base_addr->ICR |= I2C_ICR_NACKCF;
            return false;
        }

        base_addr->TXDR = byte;
    }
    if (timed_out(I2C_ISR_STOPF))
    {
        return false;
    }
    base_addr->ICR |= I2C_ICR_STOPCF;
    return true;
}

}  // namespace Stmh7
}  // namespace LBR