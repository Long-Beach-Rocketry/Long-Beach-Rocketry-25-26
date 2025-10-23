#include "bno055_imu.h"

BNO_IMU::BNO_IMU(HwI2c& i2c) : i2c(i2c) {};

void condig_imu()
{
    BNO_IMU::i2c.init();
    //setting up sensor fusion
    //not actuall function
    i2c_write(OPR_NDOF_MODE, OPR_MODE_ADDR);
}

void get_data()
{
    uint8_t accel_data_x[2];  //MSB and LSB
    uint8_t accel_data_y[2];  //...

    uint8_t gyro_data_x[2];  //MSB and LSB
    uint8_t gyro_data_y[2];  //...

    //not actual i2c function
    i2c_read(accel_data_x, ACCEL_X_LSB);
    i2c_read(accel_data_y, ACCEL_Y_LSB);

    i2c_read(gyro_data_x, GRYO_X_LSB);
    i2c_read(gyro_data_y, GRYO_Y_LSB);

    BNO_IMU::accel_x = ((int16_t)(accel_data_x[1] << 8) | accel_data_x[0]);
    BNO_IMU::accel_y = ((int16_t)(accel_data_y[1] << 8) | accel_data_y[0]);

    BNO_IMU::gyro_x = ((int16_t)(gyro_data_x[1] << 8) | gyro_data_x[0]);
    BNO_IMU::gyro_y = ((int16_t)(gyro_data_y[1] << 8) | gyro_data_y[0]);
}
