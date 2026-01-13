/**
 * @file main.cc
 * @brief PPS module test application
 * @note This application tests the PPS module functionality.
 * @author Bex Saw
 * @date 2025/12/31
 */

#include "pps.h"


using LBR::Pps;
LBR::Bno055Data imu;

int main() {
    Pps pps;
    while (true) {
        // Simulate fetching IMU data
        pps.fetchImuData(imu);
        // Update PPS state machine
        pps.update(); 
    }
    return 0;
}