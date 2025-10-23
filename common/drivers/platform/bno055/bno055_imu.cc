/** 
* @brief BNO055 IMU sensor implementation
* @file bno055_imu.cc
* @author Bex Saw
* @date 2025-10-22
*/

#include "bno055_imu.h"
#include "delay.h"
#include "st_i2c.h"

using namespace LBR::Utils;

namespace LBR {

Bno055::Bno055(LBR::Stml4::HwI2c& i2c, uint8_t address) 
    : i2c_(i2c), address_(address << 1) {} //Check after Yshi finalized the driver


/**
* @brief Initialize the BNO055 sensor
* This function sets up the sensor for operation, including setting the power mode and operation mode.
*/

void Bno055::Init() {
    
    uint8_t id = 0;

    DelayMs(650); // Wait for sensor to power up

    id = ReadingRegister(REG_CHIP_ID); // Read chip ID register
    if (id != 0xA0) {
        return;
    }

    // Go to CONFIGMODE
    SetMode(MODE_CONFIG);
    DelayMs(25);

    // Normal power mode
    WriteRegister(REG_PWR_MODE, PWR_MODE_NORMAL);
    DelayMs(10);

    // Select page 0 for main registers
    WriteRegister(REG_PAGE_ID, 0, 0x00);

    // Enter NDOF fusion mode
    SetMode(MODE_NDOF);
    DelayMs(20);

}

/**
* @brief Update sensor readings
* This function reads the latest data from the sensor.
*/

void Bno055::Update() {
    ReadMultipleRegisters(REG_ACC_START, accelData_, 6);
    ReadMultipleRegisters(REG_GYR_START, gyroData_, 6);
}

/**
* @brief Get acceleration data
* This function retrieves the acceleration data from the sensor and converts it to m/s^2.
*/

void Bno055::GetAcceleration(float& x, float& y, float& z) {

    uint8_t buf[6];
    ReadRegisters(REG_ACC_START, buf, 6);

    // Convert raw accelerometer data to m/s^2
    int16_t x = static_cast<int16_t>((buf[1] << 8) | buf[0]);
    int16_t y = static_cast<int16_t>((buf[3] << 8) | buf[2]);
    int16_t z = static_cast<int16_t>((buf[5] << 8) | buf[4]);

    // 1 LSB = 1 m/s^2 / 100 per datasheet 
    x = x / 100.0f; // Assuming 1 LSB = 0.01 m/s^2
    y = y / 100.0f;
    z = z / 100.0f;
}
 

/**
* @brief Get gyroscope data
* This function retrieves the gyroscope data from the sensor and converts it to degrees per second.
*/

void Bno055::GetGyroscope(float& x, float& y, float& z) {

    uint8_t buf[6];
    ReadMultipleRegisters(REG_GYR_START, buf, 6);

    // Convert raw gyroscope data to degrees per second
    int16_t x = static_cast<int16_t>((buf[1]<< 8)  | buf[0]);
    int16_t y = static_cast<int16_t>((buf[3] << 8) | buf[2]);
    int16_t z = static_cast<int16_t>((buf[5] << 8) | buf[4]);

    // 1 LSB = 1 dps / 16 per deg/s
    x = x / 16.0f; // Assuming 1 LSB = 0.0625 deg/s
    y = y / 16.0f;
    z = z / 16.0f;

}

/**
* @brief Update sensor fusion data
* This function updates the sensor fusion data if applicable.
*/

void Bno055::SensorFusionUpdate() {
    
}

/**
* @brief Calibrate the sensor
* This function performs sensor calibration if necessary.
*/

void Bno055::Calibrate() {

}

/**
* @brief Set the operation mode of the BNO055
* @param mode The desired operation mode
* This function writes to the operation mode register to change the sensor's mode.
*/

void Bno055::SetMode(Mode mode) {
    WriteRegister(REG_OPR_MODE, static_cast<uint8_t>(mode));
    DelayMs(30); // Delay for mode switch
}

/**
* @brief Write a value to a register on the BNO055
* @param reg The register address to write to
* @param value The value to write to the register
* This function sends a write command over I2C to the specified register.
*/

void Bno055::WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_.write(std::span<const uint8_t>(data, 2), address_);
}

/**
* @brief Read a single register from the BNO055
* @param reg The register address to read from 
* @return The value read from the register
*/

uint8_t Bno055::ReadRegister(uint8_t reg) {
    uint8_t value = 0;
    uint8_t regAddr = reg;
    i2c_.write(std::span<const uint8_t>(&regAddr, 1), address_);
    i2c_.read(std::span<uint8_t>(&value, 1), address_);
    return value;
}

/**
* @brief Read multiple registers from the BNO055
* @param startReg The starting register address to read from
* @param buffer The buffer to store the read data
* @param length The number of bytes to read
* This function reads multiple consecutive registers over I2C.
*/

void Bno055::ReadMultipleRegisters(uint8_t startReg, uint8_t* buffer, size_t length) {
    uint8_t regAddr = startReg;
    i2c_.write(std::span<const uint8_t>(&regAddr, 1), address_);
    i2c_.read(std::span<uint8_t>(buffer, length), address_);
}

} // namespace LBR