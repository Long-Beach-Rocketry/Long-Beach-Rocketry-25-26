/**
 * @file main.cc
 * @brief PPS module test application
 * @note This application tests the PPS module functionality.
 * @author Bex Saw
 * @date 2025/12/31
 */

#include "pps.h"


using LBR::Pps;

int main() {
    Pps pps;
    while (true) {
        LBR::Bno055Data imu_data;
        // TODO: Read IMU and fill imu_data, e.g.:
        // imu.read_all(imu_data);
        pps.update(imu_data); 
        // Add scheduling delay (come with testing prototype)
    }
    return 0;
}