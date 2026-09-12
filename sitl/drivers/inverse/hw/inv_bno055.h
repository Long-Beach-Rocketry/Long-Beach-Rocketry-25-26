#pragma once
#include <algorithm>
#include <unordered_map>
#include "inv_i2c_sensor.h"

namespace LBR
{

/* From inv_i2c_sensor in jc-fake-i2c which has not been merged yet*/

// enum class I2cSensorStatus : uint8_t
// {
//     OK = 0,
//     ERROR_READ,
//     ERROR_WRITE,
//     ERROR_OPCODE,
//     ERROR_DEVICE_ADDRESS,
// };
//
// struct RegisterDef
// {
//     uint8_t address;
//     uint8_t reset_value;
//     uint8_t data;
//     const char* name;
//     bool read_only;
// };

constexpr uint8_t Bno055_CHIP_ID_REG{0x00U};
constexpr uint8_t Bn055_NULL_BYTE{0x00U};

class InvBno055 : public InvI2cSensor
{
public:
    explicit InvBno055(uint8_t dev_addr);

    /**
     * @brief Initializes the register map of the Bno055
     * @return true
     */
    bool init();

    /**
     * @brief Processes the request to read from the fake i2c bus.
     * 
     * @param reg_addr The register address to read from
     * @param data The data that will contain the result of the read
     * @return true 
     * 
     * @note Devices will typically send back 0x00 
     * if the requested register address does not exist. 
     * Maybe we return false instead since this is SITL. Not sure yet.  
     */
    I2cSensorStatus process_read(const uint8_t reg_addr,
                                 uint8_t& data) override;

    /**
     * @brief Processes the request to read from the fake i2c bus on the
     * last interacted with register.
     * 
     * @param data The data that will contain the result of the read
     * @return true 
     * 
     * @note Bno055 sends back 0x00 
     * if the requested register address does not exist. 
     * Maybe we return false instead since this is SITL. Not sure yet.  
     */
    I2cSensorStatus process_read(uint8_t& data) override;

    /**
     * @brief Processes the request to write from the fake i2c bus.
     * 
     * @param reg_addr The register address to write into.
     * @param data The data that will be written to the register
     * @return true 
     * 
     * @note Bno055 silently moves on
     * if the requested register address does not exist. 
     * Maybe we return false instead since this is SITL. Not sure yet.  
     */
    I2cSensorStatus process_write(const uint8_t reg_addr,
                                  uint8_t data) override;

    /**
     * @brief Processes the request to write from the fake i2c bus on the
     * last interacted with register.
     * 
     * @param data The data that will be written to the register
     * @return true 
     * 
     * @note Bno055 silently moves on
     * if the requested register address does not exist. 
     * Maybe we return false instead since this is SITL. Not sure yet.  
     */
    I2cSensorStatus process_write(uint8_t data) override;

    /**
     * @brief Fetches the device address of the Bno055
     */
    I2cSensorStatus get_8bit_addr(uint8_t& addr) override;

private:
    uint8_t dev_addr_;
    uint8_t recent_reg_{Bno055_CHIP_ID_REG};
    std::vector<RegisterDef> register_map_;
};

}  // namespace LBR