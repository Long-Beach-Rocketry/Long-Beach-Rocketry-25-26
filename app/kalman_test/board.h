#pragma once
#include <Eigen/Dense>
#include "bno055_imu.h"
#include "bmp390.h"
#include "sys_clock.h"
#include "altitude.h"

namespace LBR
{

struct Board {
    Clock& clock;
    Bno055& bno055;
    Bmp390& bmp390; 
};

struct AvgBias{
    double accel_bias;
    double altitude_bias;
};

//For csv file processing into a sample array
struct Info {
    int time_ms;
    Eigen::Matrix<double, 2, 1> z_meas;
    Eigen::Matrix<double, 3, 1> x_state;
};

bool board_init();
Board& get_board();

}