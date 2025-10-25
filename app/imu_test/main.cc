/** 
* @file main.cc
* @brief Main file for IMU test application.
* @author Bex Saw
* @date 2025-10-24
*/

#include "imu_app_bsp.h"
#include "delay.h"
#include "stm32l476xx.h"



int main(void) {

    auto& imu = BSP_Init_IMU(I2C1, GPIOB);

    float ax = 0, ay = 0, az = 0;
    float gx = 0, gy = 0, gz = 0;

    while (1) {

        imu.GetAcceleration(ax, ay, az);
        imu.GetGyroscope(gx, gy, gz);
        DelayMs(100);

        imu.SensorFusionUpdate();
        imu.GetEuler(heading, roll, pitch);
        uinit8_t calibStatus = imu.Calibrate();
        DelayMs(100);
    }
}


// Test
// Need to pull yb-i2c after her modified driver is finalized