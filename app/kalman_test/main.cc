#include <iostream>
#include <Eigen/Dense>
#include "kalman.h"
#include "board.h"
using namespace Eigen;

namespace LBR
{
int main() {
    board_init();

    Board& hw = get_board();

    double dt = 0.01; //Period = 10ms
    Matrix<double, 10, 10>& F;
    Matrix<double, 6, 10>& H;
    Matrix<double, 10, 10>& Q;
    Matrix<double, 6, 6>& R;

    F.setIdentity();
    F(0,1) = dt;
    F(0,2) = dt^2 / 2;
    F(1,2) = dt;
    std::cout << "F matrix \n" << F << std::endl;

    H.setZero();
    H(0,1) = 1; //Select pz out of state x
    H.block<5,5>(1,2) = Matrix5d::Identity();
    //Print to see if it is the correct matrix
    std::cout << "H matrix \n" << H << std::endl;

    //Q and R need tuning via testing
    Q.diagonal()<<6.75e-5*pow(dt,4), 1.35e-4*pow(dt,2), 0.65*dt,
        7.5e-7*pow(dt,2)/4, 7.5e-7*pow(dt,2)/4, 7.5e-7*pow(dt,2)/4, 7.5e-7*pow(dt,2)/4,
        3.0e-6, 3.0e-6, 3.0e-6;

    R.diagonal()<<0.0056, 0.65
                  7.5e-7, 7.5e-7, 7.5e-7, 7.5e-7;

    EKF ekFilter(dt, F, H, Q, R);
    
    Matrix<10, 1, double> x0;
    Matrix<10, 10, double> P0;
    x0.setZero();
    x0(3) = 1; //Set q1 = 1 for a standing still rocket at 90deg
    P0.diagonal()<<1.0, 0.01, 0.001,
                   0.0025,0.0025,0.0025,1e-6,
                   1e-4,1e-4,1e-4;
    ekFilter.init(x0, P0);

    Bno055Data data;
    Bmp390 bmp;

    double gx, gy, gz, ax, ay, az, qw, qx, qy, qz;
    hw.imu.read_all(data);

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
    double az_rot = az; //Placeholder

    double baro_measure = hw.Bmp390.get_pressure();
    //To-do: get the mathematical formula to conver baro_meas to height
    double altitude = baro_measure; //Placeholder

    Vector6d z;
    z(0) = altitude;
    z(1) = az_rot;
    z(2) = qw;
    z(3) = qx;
    z(4) = qy;
    z(5) = qz;

    ekFilter.predict(gyromeasure);
    ekFilter.update(z);
}
}