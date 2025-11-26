/** 
* @file main.cc
* @brief Main file for IMU test application.
* @author Bex Saw
* @date 2025-10-24
*/

#include "board.h"
#include "delay.h"
#include <cstdlib>

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
    LBR::bsp_init();
    LBR::Board hw = LBR::get_board();
    
    uint8_t chip_id;
    hw.imu.get_chip_id(chip_id); // Expected 0xA0 for BNO055
    
    LBR::Bno055Data data;

    while (1) {
        hw.imu.read_all(data);
        LBR::Utils::DelayMs(100);
    }

}