#include "st_i2c.h"
  // Read SR1 to clear the start bit SB
  // Read SR2 to clear the address acknowledge bit ADDR
  //
  // | 1 for read
  // | 0 for write
namespace LBR
{
namespace Stmh7
{

  HwI2c::HwI2c(const StI2cParams& params) 
    : m_base_addr{params.base_addr}, m_timingr{params.timingr}
  {}

  bool HwI2c::init() {
    if (m_base_addr) {
      m_base_addr->CR1 &=~I2C_CR1_PE;

      m_base_addr->TIMINGR = m_timingr;

      // _base_addr->CR2 &= ~I2C_CR2_ADD10;  // 7-bit addressing mode

      m_base_addr->CR1 |= I2C_CR1_PE;


      return true;
    }
    return false;
  }

  bool HwI2c::mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                uint8_t dev_addr) {

  }

  bool HwI2c::mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                uint8_t dev_addr) {

  }

  bool HwI2c::mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                uint8_t dev_addr) {

  }

  bool HwI2c::mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                uint8_t dev_addr) {

  }

  bool HwI2c::read(std::span<uint8_t> data, uint8_t dev_addr) {

  }

  bool HwI2c::write(std::span<const uint8_t> data, uint8_t dev_addr) {
    
  }

}  // namespace Stmh7
}  // namespace LBR