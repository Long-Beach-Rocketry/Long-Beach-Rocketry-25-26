/**
 * @file imu_math.h
 * @brief Common math types for IMU and sensor data
 * @author Bex Saw
 * @date 2025-12-09
 */


#pragma once
#ifndef LBR_IMU_MATH_H
#define LBR_IMU_MATH_H

namespace LBR {

struct Vec3 {
    float x, y, z;
};

struct Quaternion {
    float w, x, y, z;
};


} // namespace LBR

#endif // LBR_IMU_MATH_H
