/**
* @file imu.h
* @brief Inertial Measurement Unit (IMU) sensor interface
* @author Bex Saw
* @date 2025-10-22
* @note This file contains the interface for the IMU sensor, including initialization, reading, and calibration functions.
*/

#pragma once
#include <cstdint>

namespace LBR 
{
class Imu
{
public:
    /**
    * @brief - Initializes the IMU hardware
    *        - Research IMU data (poll or DMA)
    *        - Get Acceleration in m/s^2
    *        - Get Angular velocity in degrees/s
    *        - Get Euler angles in degrees
    *        - Calibrate the IMU
    *        - Virtual destructor
    *
    */
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void GetAcceleration(float& x, float& y, float& z) = 0;
    virtual void GetGyroscope(float& x, float& y, float& z) = 0;
    virtual void SensorFusionUpdate() = 0; //Check on this soon
    virtual void Calibrate() = 0; // Maybe
    virtual ~Imu() = default;
}
} // namespace LBR