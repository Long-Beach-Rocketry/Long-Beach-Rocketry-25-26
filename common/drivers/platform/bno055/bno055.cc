/** 
* @brief BNO055 IMU sensor implementation
* @file bno055.cc
* @author Bex Saw
* @date 2025-10-22
*/

#include "bno055.h"
#include "delay.h"

using namespace LBR::Utils;

namespace LBR {

Bno055::Bno055(I2C_HandleTypeDef* hi2c, uint8_t address) 
    : hi2c_(hi2c), address_(address << 1) {}


// Init

void Bno055::Init() {
    // Initialize the BNO055 sensor
    // Set up I2C communication, configure sensor settings, etc.
    // This is a placeholder for actual initialization code
    uint8_t id = 0;

    DelayMs(650); // Wait for sensor to power up

    ReadingRegister(REG_CHIP_ID, &id, 1); // Read chip ID register
    if (id != 0xA0) {
        // Handle error: incorrect chip ID
        return;
    }

    // Enter CONFIGMODE before changing settings
    SetMode(MODE_CONFIG);
    DelayMs(25);

    // Normal power mode
    WriteRegister(REG_PWR_MODE, PWR_MODE_NORMAL);
    DelayMs(10);


    // Switch to NDOF mode for sensor fusion
    SetMode(MODE_NDOF);
    DelayMs(20);

}

void Bno055::Update() {
    // Update sensor readings (Not sure)
    // This is a placeholder for actual update code
    ReadMultipleRegisters(REG_ACC_START, accelData_, 6);
    ReadMultipleRegisters(REG_GYR_START, gyroData_, 6);
}

void Bno055::GetAcceleration(float& x, float& y, float& z) {

    uint8_t buf[6];
    ReadRegisters(REG_ACC_START, buf, 6);

    // Convert raw accelerometer data to m/s^2
    int16_t x = static_cast<int16_t>((buf[1] << 8) | buf[0]);
    int16_t y = static_cast<int16_t>((buf[3] << 8) | buf[2]);
    int16_t z = static_cast<int16_t>((buf[5] << 8) | buf[4]);

    x = x / 100.0f; // Assuming 1 LSB = 0.01 m/s^2
    y = y / 100.0f;
    z = z / 100.0f;
}

// TODO: GetGyroscope(), Calibrate(), SetMode(), WriteRegister(), ReadRegister(), ReadMultipleRegisters()
} // namespace LBR