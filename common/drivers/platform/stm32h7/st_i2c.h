/**
 * @file st_i2c.h
 * @brief I2C driver specfic for the stmh7.
 * @author Joseph Chang
 */

#pragma once

#include <array>
#include <cstdint>
#include "i2c.h"
#include "stm32h723xx.h"

namespace LBR
{
namespace Stmh7
{

/**
 * @brief Parameters for the i2c driver: the I2C base address, timing register, and timeout delay in ms (defaults to 1000 ms).
 */
struct StI2cParams
{
    I2C_TypeDef* base_addr;
    uint32_t timingr;
    uint32_t timeout_delay_ms = 1000;
};

class HwI2c : public I2c
{

public:
    /**
     * @brief Hw Contructor
     * @param StI2cParams address of a struct with port, timingr, and countdown delay in ms.
     */
    explicit HwI2c(const StI2cParams& params);

    /**
     * @brief Initializes I2C peripheral.
     * 
     * @return true if successful, false otherwise
     */
    bool init();

    bool mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                  uint8_t dev_addr) override;

    bool mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                  uint8_t dev_addr) override;

    bool mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                   uint8_t dev_addr) override;

    bool mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                   uint8_t dev_addr) override;

    bool read(std::span<uint8_t> data, uint8_t dev_addr) override;

    bool write(std::span<const uint8_t> data, uint8_t dev_addr) override;

private:
    /**
     * @brief Private help function that hecks if the given I2C base address is valid
     * and NOT `nullptr`.
     * 
     * @param None
     * @return `true` if valid and NOT `nullptr`, `false` otherwise
     */
    bool is_enabled();

    /**
     * @brief Private helper function that checks the busy flag of the I2C interupt status
     * register. This functions also checks if the peripheral is enabled by 
     * calling `is_enabled()`.
     * 
     * @param None
     * @return `true` if busy, `false` otherwise
     */
    bool bus_free();

    /**
     * @brief Helper timeout function that delays by the `timeout_delay_ms` to wait
     * for a slave response or complete data sending.
     * 
     * @param uint32_t A flag of the I2C interupt status register (ISR)
     * @return `true` if the passed in flag is raised.
     * 
     * `false` if the function has delayed for however long the given
     * `timeout_delay_ms` or if some I2C error flag is raised.
     */
    bool timed_out(uint32_t flag);

    I2C_TypeDef* base_addr;
    uint32_t timingr;
    uint32_t timeout_delay_ms;
};

}  // namespace Stmh7
}  // namespace LBR