#pragma once

#include "imu.h"
#include "st_i2c.h"
#include "stm32l4xx.h"

#define OPR_NDOF_MODE 0b1100;
#define DEVICE_ADDR 0x28;
#define OPR_MODE_ADDR 0x3D;
#define ACCEL_X_LSB 0x08;
#define ACCEL_X_MSB 0x09;
#define ACCEL_Y_LSB 0x0A;
#define ACCEL_Y_MSB 0x0B;
#define GRYO_X_LSB 0x14;
#define GRYO_X_MSB 0x15;
#define GRYO_Y_LSB 0x16;
#define GRYO_Y_MSB 0x17;

class BNO_IMU : public IMU
{
private:
    HwI2c& i2c;
    int16_t accel_x, accel_y, gyro_x, gyro_y;

public:
    explicit BNO_IMU(HwI2c& i2c);

    void get_data() override;

    // void calculate_data();

    void config_imu();
};