/** 
* @file main.cc
* @brief Main file for IMU test application.
* @author Bex Saw
* @date 2025-10-24
*/

#include "imu_app_bsp.h"
#include "delay.h"
#include <cstdio> 

/* 
idea:
[BNO055 Sensor] 
    ↓ (I²C @ 100kHz)
[FlightComputer]
    ↓ (printf / UART or something else not sure yet)
[USB/Serial Virtual COM Port]
    ↓ (Real-time data packets)
[Ayush’s UI Application]
*/
    
int main() {
    auto& imu = LBR::bsp_init_imu();
    
    uint8_t chip_id;
    imu.get_chip_id(chip_id);
    
    printf("I2C Address: 0x%02X\n", LBR::Bno055::ADDR_PRIMARY);
    printf("Chip ID from register 0x00: 0x%02X (expected 0xA0)\n\n", chip_id);
    
    LBR::Bno055Data data;

    while (1) {
        imu.read_all(data);

        printf("Accel:  X=%.2f Y=%.2f Z=%.2f\n", data.accel.x, data.accel.y, data.accel.z);
        printf("Gyro:   X=%.2f Y=%.2f Z=%.2f\n", data.gyro.x, data.gyro.y, data.gyro.z);
        printf("LinAcc: X=%.2f Y=%.2f Z=%.2f\n", data.linear_accel.x, data.linear_accel.y, data.linear_accel.z);
        printf("Grav:   X=%.2f Y=%.2f Z=%.2f\n", data.gravity.x, data.gravity.y, data.gravity.z);
        printf("Quat:   W=%.3f X=%.3f Y=%.3f Z=%.3f\n\n", data.quat.w, data.quat.x, data.quat.y, data.quat.z);

        LBR::Utils::DelayMs(100);
    }

}