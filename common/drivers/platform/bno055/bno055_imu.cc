/** 
* @brief BNO055 IMU sensor implementation
* @file bno055_imu.cc
* @author Bex Saw
* @date 2025-10-22
*/

#include "bno055_imu.h"
#include "delay.h"
#include "st_i2c.h"

using namespace LBR::Stml4;
using LBR::Utils::DelayMs;

namespace LBR {

Bno055::Bno055(LBR::Stml4::HwI2c& i2c, uint8_t address) 
    : i2c_(i2c), address_(address << 1) {} //Check after Yshi finalized the driver


/**
* @brief Initialize the BNO055 sensor
* This function sets up the sensor for operation, including setting the power mode and operation mode.
*/

void Bno055::Init() {
    
    DelayMs(650);  // Wait for sensor to power up
    uint8_t id = ReadRegister(REG_CHIP_ID);
    for (int i = 0; i < 5 && id != 0xA0; ++i) {
        DelayMs(10);
        id = ReadRegister(REG_CHIP_ID);
    }
    if (id != 0xA0) return;
    

    // Go to CONFIGMODE -- Default
    SetMode(MODE::CONFIG);
    DelayMs(25);
    WriteRegister(REG_PWR_MODE, static_cast<uint8_t>(PowerMode::NORMAL));
    WriteRegister(REG_PAGE_ID, 0x00);


    
    SetMode(MODE::IMU); // Back to IMU mode
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

void Bno055::GetAcceleration(float& ax, float& ay, float& az) {

    uint8_t buf[6];
    ReadMultipleRegisters(REG_ACC_START, buf, 6);

    // Convert raw accelerometer data to m/s^2
    int16_t x = (buf[1] << 8) | buf[0];
    int16_t y = (buf[3] << 8) | buf[2];
    int16_t z = (buf[5] << 8) | buf[4];

    // 1 LSB = 1 m/s^2 / 100 per datasheet 
    ax = x / 100.0f; // Assuming 1 LSB = 0.01 m/s^2
    ay = y / 100.0f;
    az = z / 100.0f;
}
 

/**
* @brief Get gyroscope data
* This function retrieves the gyroscope data from the sensor and converts it to degrees per second.
*/

void Bno055::GetGyroscope(float& gx, float& gy, float& gz) {

    uint8_t buf[6];
    ReadMultipleRegisters(REG_GYR_START, buf, 6);

    // Convert raw gyroscope data to degrees per second
    int16_t x = (buf[1] << 8) | buf[0];
    int16_t y = (buf[3] << 8) | buf[2];
    int16_t z = (buf[5] << 8) | buf[4];

    // 1 LSB = 1 dps / 16 per deg/s
    gx = x / 16.0f; // Assuming 1 LSB = 0.0625 deg/s
    gy = y / 16.0f;
    gz = z / 16.0f;

}

/**
* @brief Update sensor fusion data
* This function updates the sensor fusion data if applicable.
*/

void Bno055::SensorFusionUpdate() {
    uint8_t buf[6];
    ReadMultipleRegisters(REG_EUL_START, buf, 6);

    int16_t x = (buf[1] << 8) | buf[0];
    int16_t y = (buf[3] << 8) | buf[2];
    int16_t z = (buf[5] << 8) | buf[4];

    // Convert to degrees - 1 LSB = 1/16 degree
    euler_[0] = x / 16.0f;
    euler_[1] = y / 16.0f;
    euler_[2] = z / 16.0f;

}


/** 
* @brief Get Data For FusedEuler 
* This including: heading, roll, pitch of the payload.
*/

void Bno055::GetFusedEuler(float& heading, float& roll, float& pitch) {
    
    heading = euler_[0];
    roll    = euler_[1];
    pitch   = euler_[2];
}

/**
* @brief Calibrate the sensor
* This function performs sensor calibration if necessary.
*/

uint8_t Bno055::Calibrate() {
    uint8_t sys, gyro, accel, mag;

    uint8_t calibStat = ReadRegister(REG_CALIB_STAT);
    sys     = (calibStat >> 6) & 0x03;      
    gyro    = (calibStat >> 4) & 0x03;      
    accel   = (calibStat >> 2) & 0x03;     
    mag     = calibStat & 0x03;    
    return (sys << 6) | (gyro << 4) | (accel << 2) | mag;
}


/**
* @brief Read system status for status, error, and IsFullyCalibrated.
* @return The system status byte
*/

uint8_t Bno055::GetSystemStatus() {
    return ReadRegister(REG_SYS_STATUS);
}

/* Maybe Add?
const char* Bno055::DecodeSystemStatus(uint8_t status) {
    switch (status) {
        case 0x00: return "System idle";
        case 0x01: return "System error";
        case 0x02: return "Initializing peripherals";
        case 0x03: return "System initialization";
        case 0x04: return "Executing self-test";
        case 0x05: return "Running - fusion algorithm off";
        case 0x06: return "Running - fusion algorithm on";
        default:   return "Unknown status";
    }
} */

uint8_t Bno055::GetSystemError() {
    return ReadRegister(REG_SYS_ERR);
}

bool Bno055::IsSystemError() {
    return GetSystemError() != 0;
}

bool Bno055::IsFullyCalibrated() {
    uint8_t calib = Calibrate();
    return (calib & 0xFF) == 0xFF; // All systems calibrated
}

/**
* @brief Set the operation mode of the BNO055
* @param mode The desired operation mode
* This function writes to the operation mode register to change the sensor's mode.
*/

void Bno055::SetMode(MODE mode) {
    if(mode > MODE::IMU) return; // Invalid MODE, ignore bro
    WriteRegister(REG_OPR_MODE, static_cast<uint8_t>(mode));
    DelayMs(30); // Delay for mode switch
}

/**
* @brief Write a value to a register on the BNO055
* @param reg The register address to write to
* @param value The value to write to the register
* This function sends a write command over Yshi I2C to the specified register.
*/

void Bno055::WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_.write(data, 2, address_);
}

/**
* @brief Read a single register from the BNO055
* @param reg The register address to read from 
* @return The value read from the register
*/

uint8_t Bno055::ReadRegister(uint8_t reg) {
    uint8_t value = 0;
    i2c_.write(&reg, 1, address_);
    i2c_.read(&value, 1, address_);
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
    i2c_.write(&startReg, 1, address_);
    i2c_.read(buffer, length, address_);
}

/**
* @brief One of the Function that get the LinearAcceleration with Gravity removed
* - May Remove or Keep Depends on TJ
* @note Detecting launch. burnout, seperation events,
*       it can also estimating velocitu or altitude chagnes when fused 
*       -with barometer or gyoscope data. 
*/

void Bno055::GetLinearAcceleration(float& lax, float& lay, float& laz) {
    uint8_t buf[6];
    ReadMultipleRegisters(REG_LIA_ACCEL_X_LSB, buf, 6);

    int16_t x = (buf[1] << 8) | buf[0];
    int16_t y = (buf[3] << 8) | buf[2];
    int16_t z = (buf[5] << 8) | buf[4];

    // 1 LSB = 1 m/s^2 / 100 per datasheet 
    lax = x / 100.0f; // Assuming 1 LSB = 0.01 m/s^2
    lay = y / 100.0f;
    laz = z / 100.0f;
}

/**
* @brief Gives the direction and magnitude of gravity relative to the IMU's orientation.
* @note It tells you which way "down" is in the sensors reference frame.
* - Benefit: Detecting free-fall and determine orientation/tilt relative to the ground
*/

void Bno055::GetGravityVector(float& gx, float& gy, float& gz) {
    uint8_t buf[6];
    ReadMultipleRegisters(REG_GRAVITY_X_LSB, buf, 6);

    int16_t x = (buf[1] << 8) | buf[0];
    int16_t y = (buf[3] << 8) | buf[2];
    int16_t z = (buf[5] << 8) | buf[4];

    // 1 LSB = 1 m/s^2 / 100 per datasheet 
    gx = x / 100.0f; // Assuming 1 LSB = 0.01 m/s^2
    gy = y / 100.0f;
    gz = z / 100.0f;

}

/* Self Tests */

/** 
* @brief checks if the IMU's internal senrsors or MCU are function
* @note This is for StartUp Validation, Pre-Launch diagnostic, Fault Detection, etc.
*/

uint8_t Bno055::RunPowerOnSelfTest() {
     uint8_t result = ReadRegister(REG_ST_RESULT);

     bool mcu_pass  = (result >> 3) & 0x01;
     bool gyro_pass = (result >> 2) & 0x01;
     bool acc_pass  = (result >> 0) & 0x01;

     if(!mcu_pass || !gyro_pass || !acc_pass ) {
        if(!mcu_pass)   printf("BNO055 Self-Test FAIL: MCU\n");
        if(!gyro_pass)  printf("BNO055 Self-Test FAIL: Gyroscope\n");
        if(!acc_pass)   printf("BNO055 Self-Test FAIL: Accelerometer\n");
     } else {
        printf("BNO055 Self-Test PASS (MCU, Gyro, Accel OK)\n");
     }

     return result;

    } 

/* This is Optional treat it as a manual call to see */

/** 
* @brief manually triggers the IMU's internal diagnostic rountine while the system is running
*/


uint8_t Bno055::RunBuiltInSelfTest() { 
    // Trigger the built-in self-test
    SetMode(MODE::CONFIG);
    DelayMs(25);

    uint8_t sys_trigger = ReadRegister(REG_SYS_TRIGGER);
    sys_trigger |= 0x01; // Set the self-test bit
    WriteRegister(REG_SYS_TRIGGER, sys_trigger);

    DelayMs(1000); // Wait for self-test to complete
    uint8_t result = ReadRegister(REG_ST_RESULT);

    // Return to IMU operation mode
    SetMode(MODE::IMU);
    DelayMs(20);

    return result;
}
} // namespace LBR

