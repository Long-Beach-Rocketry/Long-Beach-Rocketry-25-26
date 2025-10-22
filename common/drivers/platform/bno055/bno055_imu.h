#pragma once

#include "imu.h"

class BNO_IMU : public IMU
{
public:
    void get_data() override;

    void calculate_data();
};