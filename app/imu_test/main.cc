/** 
* @file main.cc
* @brief Main file for IMU test application.
* @author Bex Saw
* @date 2025-10-24
*/

#include "imu_app_bsp.h"
#include "delay.h"
#include <cstdio> 

// Need to pull yb-i2c after her modified driver is finalized
/* This is for Ayush Packet to stream data to his UI interface 
struct IMU_Packet { 
    float ax, ay, az;       // acceleration 
    float gx, gy, gz;       // gyroscope 
    float roll, pitch, yaw; // fused Euler orientation
    float lax, lay, laz;    // LinearAccel
    float gvx, gvy, gvz;    // GravityVector
};

idea:
[BNO055 Sensor] 
    ↓ (I²C @ 100kHz)
[FlightComputer]
    ↓ (printf / UART or something else not sure yet)
[USB/Serial Virtual COM Port]
    ↓ (Real-time data packets)
[Ayush’s UI Application]
*/

int main(void) {
    // 1. Initialize Board & IMU
    auto& imu = BSP_Init_IMU(I2C1, GPIOB);
    DelayMs(100);

    printf("BNO055 IMU Initialization\n");

    // 2. Self Test Instance 
    uint8_t post_result = imu.RunPowerOnSelfTest();
    printf("POST Test: 0x%02X\n", post_result);

    uint8_t bist_result = imu.RunBuildInSelfTest();
    printf("BIST Result: 0x%02X\n\n", bist_result);

    // 3. Variables to hold sensor data (for Ayush)
    // IMU_Packet pkt = {0};

    // 4. Main Loop (10Hz)
    while (true) {

        imu.Update();

        // 5. Read all Sensors
        imu.GetAcceleration(ax, ay, az);
        imu.GetGyroscope(gx, gy, gz);
        imu.SensorFusionUpdate();
        imu.GetFusedEuler(roll, pitch, heading); // Actual Order: heading, roll, pitch
        
        // * Optional (Don't know yet)
        imu.GetLinearAcceleration(lax, lay, laz);
        imu.GetGravityVector(gvx, gvy, gvz);

        // 6. Check System Status
        uint8_t sys_err = imu.GetSystemError();
        if (sys_err != 0) {
            printf("IMU ERROR CODE: 0x%02X\n", sys_err)
        }
        // 7. System Calibration Update 
        uint8_t calibStatus = imu.Calibrate();


        // For Ayush (need to talk since his UI stream the waveform)
        // This is for the data packet 
        /*printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
               pkt.ax, pkt.ay, pkt.az,
               pkt.gx, pkt.gy, pkt.gz,
               pkt.yaw, pkt.roll, pkt.pitch); */

        // 8. Wait 100 ms -> 10 Hz
        DelayMs(100);
    }

}

