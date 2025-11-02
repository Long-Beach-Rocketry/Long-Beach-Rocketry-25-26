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
    float roll = 0, pitch = 0, heading = 0;
    float lax = 0, lay = 0, laz = 0;
    float gvx = 0, gvy = 0, gvz = 0;

    uint8_t calibStatus = 0;
    // Self-test results (?)
    uint8_t postResult = imu.RunPowerOnSelfTest(); 
    uint8_t bistResult = imu.RunBuiltInSelfTest();

    while (1) {

        imu.GetAcceleration(ax, ay, az);
        imu.GetGyroscope(gx, gy, gz);

        imu.SensorFusionUpdate();
        imu.GetFusedEuler(roll, pitch, heading);

        imu.GetLinearAcceleration(lax, lay, laz);
        imu.GetGravityVector(gvx, gvy, gvz);

        DelayMs(100);


        calibStatus = imu.Calibrate();

        DelayMs(100);

        
    }
}


// Test
// Need to pull yb-i2c after her modified driver is finalized