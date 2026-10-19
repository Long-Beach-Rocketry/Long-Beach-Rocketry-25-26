//#include <iostream>
#include <Eigen/Dense>
#include <cmath>
#include "board.h"
#include "kalman.h"

using namespace Eigen;
using namespace std;
using namespace LBR;

static constexpr double SEA_LEVEL_PA{101325};

double gx, gy, gz, ax, ay, az, qw, qx, qy, qz;

int main()
{
    board_init();

    Board& hw = get_board();

    double dt = 0.01;  //Period = 10ms
    Matrix<double, 10, 10> F;
    Matrix<double, 6, 10> H;
    Matrix<double, 10, 10> Q;
    Matrix<double, 6, 6> R;

    //Only the relationship between pz, vz, az
    // Quaternion is handled separately in predict
    F.setIdentity();
    F(0, 1) = dt;
    F(0, 2) = pow(dt, 2) / 2;
    F(1, 2) = dt;

    H.setZero();
    H(0, 1) = 1;  //Select pz out of state x
    H(1, 2) = 1;  //az
    H(2, 3) = 1;  //qw
    H(3, 4) = 1;  //qx
    H(4, 5) = 1;  //qy
    H(5, 6) = 1;  //qz

    //Q and R need tuning via testing
    Q.diagonal() << 6.75e-5 * pow(dt, 4), 1.35e-4 * pow(dt, 2), 0.65 * dt,
        7.5e-7 * pow(dt, 2) / 4, 7.5e-7 * pow(dt, 2) / 4,
        7.5e-7 * pow(dt, 2) / 4, 7.5e-7 * pow(dt, 2) / 4, 3.0e-6, 3.0e-6,
        3.0e-6;

    R.diagonal() << 0.0056, 0.65, 7.5e-7, 7.5e-7, 7.5e-7, 7.5e-7;

    EKF ekFilter(dt, F, H, Q, R);

    Matrix<double, 10, 1> x0;
    Matrix<double, 10, 10> P0;
    x0.setZero();
    x0(3) = 1;  //Set q1 = 1 for a standing still rocket at 90deg
    P0.diagonal() << 1.0, 0.01, 0.001, 0.0025, 0.0025, 0.0025, 1e-6, 1e-4, 1e-4,
        1e-4;
    ekFilter.initialize(x0, P0);

    Bno055Data data;
    Matrix<double, 10, 1> state;
    while (1)
    {
        hw.bno055.read_all(data);

        //Gyroscope reading
        gx = data.gyro.x;
        gy = data.gyro.y;
        gz = data.gyro.z;
        Vector3d gyromeasure;
        gyromeasure << gx, gy, gz;

        //Quaternion reading
        qw = data.quat.w;
        qx = data.quat.x;
        qy = data.quat.y;
        qz = data.quat.z;

        //Linear accceleration reading
        ax = data.linear_accel.x;
        ay = data.linear_accel.y;
        az = data.linear_accel.z;

        //To-do rotate the az_sensor to z_real_world
        // This is due to wind resistance that can cause yaw, pitch, roll motion,
        // which rotate the imu and require to correct its internal axis to real world
        // Solution: use the Euler-Rodrigues 3d rotation with quaternion measurement
        double az_rot = 2 * (qx * qz - qw * qy) * ax +
                        2 * ay * (qy * qz - qw * qx) +
                        az * (1 - 2 * (pow(qx, 2) + pow(qy, 2)));

        double baro_measure = hw.bmp390.get_pressure();
        double altitude = cal_altitude(baro_measure, SEA_LEVEL_PA);

        Matrix<double, 6, 1> z;
        z(0) = altitude;
        z(1) = az_rot;
        z(2) = qw;
        z(3) = qx;
        z(4) = qy;
        z(5) = qz;

        ekFilter.predict(gyromeasure);
        ekFilter.update(z);
        
        state = ekFilter.getXhat();

    }

    return 0;
}