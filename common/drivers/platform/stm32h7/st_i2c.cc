#include "st_i2c.h"

// | 1 for read
// | 0 for write

  namespace LBR
{
namespace Stmh7
{

  HwI2c::HwI2c(const StI2cParams& params) 
    : m_base_addr{params.base_addr}, m_timingr{params.timingr}, m_timeout_delay{params.timeout_delay}
  {}

  bool HwI2c::init() {
    if (m_base_addr) {
      m_base_addr->CR1 &=~I2C_CR1_PE;

      m_base_addr->TIMINGR = m_timingr;

      m_base_addr->CR2 &= ~I2C_CR2_ADD10;  // 7-bit addressing mode

      m_base_addr->CR1 |= I2C_CR1_PE;

      return true;
    }
    return false;
  }

/* Private Helpers */

  bool HwI2c::isEnabled() {
    if (m_base_addr) {
      return (m_base_addr->CR1 & I2C_CR1_PE);
    }
    return false;
  }
  
  bool HwI2c::busFree() {
    if (isEnabled()) {
      return !(m_base_addr->ISR & I2C_ISR_BUSY);
    }
    return false;
  }

  bool HwI2c::timedOut(uint32_t flag) {
    uint32_t timeout = m_timeout_delay;
    while (--timeout) {
      if (m_base_addr->ISR & (I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO)) {
        m_base_addr->ICR |= I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF;
        return true;
      }
      if (flag & m_base_addr->ISR) {
        return false;
      }
    }
    return true;
  }


/* Public methods */

  bool HwI2c::mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                uint8_t dev_addr) {
    /* Check if ready */
    if (!busFree()) { return false; }

    /* Write slave address, Write mode */
    m_base_addr->CR2 &=~ (I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND);
    m_base_addr->CR2 |= ((dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START | ((8/8) << I2C_CR2_NBYTES_Pos));

    /* Transmit register address */
    if (timedOut(I2C_ISR_TXIS)) { return false; }
    m_base_addr->TXDR = reg_addr;

    if (timedOut(I2C_ISR_TC)) { return false; }

    /* Write slave address, Read mode */
    m_base_addr->CR2 &=~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND);
    m_base_addr->CR2 |= ((dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_RD_WRN | I2C_CR2_START | (data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND);

    /* Read loop */
    for (uint8_t& byte : data) {
      if (timedOut(I2C_ISR_RXNE)) { return false; }

      byte = m_base_addr->RXDR;
    }

    if (timedOut(I2C_ISR_STOPF)) { return false; }

    m_base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
  }

  bool HwI2c::mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                uint8_t dev_addr) {
    /* Check if ready */
    if (!busFree()) { return false; }

    /* Write slave address, Write mode */
    m_base_addr->CR2 &=~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND);
    m_base_addr->CR2 |= ((dev_addr << (1 + I2C_CR2_SADD_Pos)) | I2C_CR2_START | ((16/8) << I2C_CR2_NBYTES_Pos));

    /* Transmit both halves of the register address */
    if (timedOut(I2C_ISR_TXIS)) { return false; }
    m_base_addr->TXDR = static_cast<uint8_t>((reg_addr >> 8) & 0xFF); // Upper half

    if (timedOut(I2C_ISR_TXIS)) { return false; }
    m_base_addr->TXDR = static_cast<uint8_t>(reg_addr & 0xFF); // Lower half

    if (timedOut(I2C_ISR_TC)) { return false; }

    /* Write slave address, Read mode */
    m_base_addr->CR2 &=~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND);
    m_base_addr->CR2 |= ((dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_RD_WRN | I2C_CR2_START | (data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND);

    /* Read loop */
    for (uint8_t& byte : data) {
      if (timedOut(I2C_ISR_RXNE)) { return false; }

      byte = m_base_addr->RXDR;
    }
    if (timedOut(I2C_ISR_STOPF)) { return false; }
    m_base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
  }

  bool HwI2c::mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                uint8_t dev_addr) {
    /* Check if ready */
    if (!busFree()) { return false; }

    /* Write slave address, Write mode */
    m_base_addr->CR2 &=~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND);
    m_base_addr->CR2 |= ((dev_addr << (1 + I2C_CR2_SADD_Pos)) | I2C_CR2_START | ((8/8 + data.size()) << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND);

    if (timedOut(I2C_ISR_TXIS)) { return false; }

    /* Transmit register address */
    m_base_addr->TXDR = reg_addr;

    /* Write loop */
    for (const uint8_t byte : data) {
      if (timedOut(I2C_ISR_TXIS)) { return false; }
      if (m_base_addr->ISR & I2C_ISR_NACKF) {
        m_base_addr->ICR |= I2C_ICR_NACKCF;
        return false;
      }

      m_base_addr->TXDR = byte;
    }

    if (timedOut(I2C_ISR_STOPF)) { return false; }
    m_base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
  }

  bool HwI2c::mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                uint8_t dev_addr) {
    /* Check if ready */
    if (!busFree()) { return false; }

    /* Write slave address, Write mode */
    m_base_addr->CR2 &=~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND);
    m_base_addr->CR2 |= ((dev_addr << (1 + I2C_CR2_SADD_Pos)) | I2C_CR2_START | ((data.size() + 16/8) << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND);

    /* Transmit both halves of the register address */
    if (timedOut(I2C_ISR_TXIS)) { return false; }
    m_base_addr->TXDR = static_cast<uint8_t>((reg_addr >> 8) & 0xFF); // Upper half

    if (timedOut(I2C_ISR_TXIS)) { return false; }
    m_base_addr->TXDR = static_cast<uint8_t>(reg_addr & 0xFF); // Lower half

    /* Write loop */
    for (const uint8_t byte : data) {
      if (timedOut(I2C_ISR_TXIS)) { return false; }
      if (m_base_addr->ISR & I2C_ISR_NACKF) { 
        m_base_addr->ICR |= I2C_ICR_NACKCF;
        return false;
      }

      m_base_addr->TXDR = byte;
    }

    if (timedOut(I2C_ISR_STOPF)) { return false; }
    m_base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
  }

  bool HwI2c::read(std::span<uint8_t> data, uint8_t dev_addr) {
    /* Check if ready */
    if (!(busFree())) { return false; }

    /* Write slave address, Read mode */
    m_base_addr->CR2 &=~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND);
    m_base_addr->CR2 |= ((dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_RD_WRN | I2C_CR2_START | (data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND);

    /* Read loop */
    for (uint8_t& byte: data) {
      if (timedOut(I2C_ISR_RXNE)) { return false; }

      byte = m_base_addr->RXDR;
    }

    if (timedOut(I2C_ISR_STOPF)) { return false; }
    m_base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
  }

  bool HwI2c::write(std::span<const uint8_t> data, uint8_t dev_addr) {
    /* Check if ready */
    if (!busFree()) { return false; }

    /* Write slave address, Write mode */
    m_base_addr->CR2 &=~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND);
    m_base_addr->CR2 |= ((dev_addr << (1 + I2C_CR2_SADD_Pos)) | I2C_CR2_START | (data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND);

    /* Write loop */
    for (const uint8_t byte : data) {

      if (timedOut(I2C_ISR_TXIS)) { return false; }
      if (I2C_ISR_NACKF & m_base_addr->ISR) {
        m_base_addr->ICR |= I2C_ICR_NACKCF;
        return false;
      }

      m_base_addr->TXDR = byte;
    }
    if (timedOut(I2C_ISR_STOPF)) { return false; }
    m_base_addr->ICR |= I2C_ICR_STOPCF;
    return true;
  }

}  // namespace Stmh7
}  // namespace LBR