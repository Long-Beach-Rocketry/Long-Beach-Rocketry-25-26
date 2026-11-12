//#include <Eigen/Dense>
#include <cerrno>  //For error tracking when open csv file
#include <cmath>
#include <cstdlib>
#include <fstream>  // For CSV file stream logging operations
#include <iomanip>
#include <iostream>  // For error stream printing
#include "board.h"
#include "kalman.h"
#include "stm32h7xx_hal.h"  // For access to the global clock function HAL_Delay()

using namespace Eigen;
using namespace std;
using namespace LBR;

static constexpr double ATMOSPH_PRESS_InHg{29.98};
static constexpr double PI{3.1415926};
static constexpr uint16_t MAX_SIZE{500};
static constexpr uint8_t MAX_CALIB_SIZE{70};


double baro_altitude(Board& hw);
AvgBias param_average(Board& hw);
Info samples[MAX_SIZE];
LBR::Bno055Data reading;
double altitude;
Matrix<double, 3, 1> kf_state;
Matrix<double, 2, 1> z;
volatile int csv_open_errno = 0;

extern "C" void initialise_monitor_handles(void);

int main()
{
    initialise_monitor_handles();
    board_init();

    Board& hw = get_board();

    Matrix<double, 3, 3> F;
    Matrix<double, 2, 3> H;
    Matrix<double, 3, 3> Q;
    Matrix<double, 2, 2> R;

    //Diagonal F is dynamic determined in predict()
    F.setZero();
    F.setIdentity();

    H.setZero();
    H << 1, 0, 0, 
        0, 0, 1;

    //Q and R need tuning via testing
    double dt = 0.02;  //Assume 50Hz for now, more is handled separately
    Q.setZero();
    Q.diagonal() << 6.75e-3 * pow(dt, 4), 1.35e-4 * pow(dt, 2), 5e-2 * dt;

    R.setZero();
    R.diagonal() << 0.1, 0.015;

    EKF ekFilter(F, H, Q, R);

    const AvgBias param_bias = param_average(hw);
    double accel_offset = param_bias.accel_bias;
    double altitude_offset = param_bias.altitude_bias;

    altitude = baro_altitude(hw) - altitude_offset;
    hw.bno055.read_all(reading);

    Matrix<double, 3, 1> x0;
    Matrix<double, 3, 3> P0;
    x0.setZero(); //Have already subtracted by the accel_offset and baro_offset
    P0.setZero();
    P0.diagonal() << 3.0, 0.01, 0.002;

    ekFilter.initialize(x0, P0);

    uint16_t sample_count = 0;

    double ax, ay, az, qw, qx, qy, qz;
    uint32_t previous_sample_ms = HAL_GetTick();

    while (sample_count < MAX_SIZE)
    //while(1)
    {
        hw.bno055.read_all(reading);

        //Quaternion reading
        qw = reading.quat.w;
        qx = reading.quat.x;
        qy = reading.quat.y;
        qz = reading.quat.z;

        //Linear accceleration reading (already subtract gravity)
        ax = reading.linear_accel.x;
        ay = reading.linear_accel.y;
        az = reading.linear_accel.z;

        // Rotate the az_sensor to z_real_world
        // This is due to wind resistance that can cause yaw, pitch, roll motion,
        // which rotate the imu and require to correct its internal axis to real world
        // Solution: use the Euler-Rodrigues 3d rotation with quaternion measurement
        double az_rot = 2 * ax * (qx * qz - qw * qy) +
                        2 * ay * (qy * qz + qw * qx) +
                        az * (1 - 2 * (pow(qx, 2) + pow(qy, 2)));

        altitude = baro_altitude(hw) - altitude_offset;

        uint32_t sample_time_ms = HAL_GetTick();
        uint32_t elapsed_ms = sample_time_ms - previous_sample_ms;

        //Check if sample_time_ms has is greater enough than previous_sample_ms
        if (elapsed_ms == 0U)
        {
            continue;
        }

        double dt_seconds = elapsed_ms * 0.001;  //In seconds

        previous_sample_ms = sample_time_ms;
        // Pass the measured interval, not the initial dt.

        //Matrix<double, 2, 1> z;
        z(0) = altitude;
        z(1) = az_rot - accel_offset;

        ekFilter.predict(dt_seconds);
        ekFilter.update(z);

        // Keep this global variable available in Watch.
        kf_state = ekFilter.getXhat();

        Info& record = samples[sample_count];

        record.time_ms = sample_time_ms;
        record.z_meas = z;
        record.x_state = kf_state;
        ++sample_count;
        //End
    }

    //Process the sample array to store it into the csv file
    //Start code here
    errno = 0;
    ofstream csv_file("flight_data.csv", ios::out | ios::trunc);
    csv_open_errno = errno;

    if (!csv_file.is_open())
    {
        // Pause here and inspect csv_open_errno.
        while (true)
        {
        }
    }

    csv_file << setprecision(15);

    csv_file
        << "time_ms,z_pz,z_az,kf_pz,kf_vz,kf_az\n";

    for (uint16_t n = 0; n < sample_count; ++n)
    {
        const Info& record = samples[n];

        csv_file
            << record.time_ms << ','
            << record.z_meas(0) << ','
            << record.z_meas(1) << ','
            << record.x_state(0) << ','
            << record.x_state(1) << ','
            << record.x_state(2) << '\n';

        if (!csv_file)
        {
            break;
        }
    }
    csv_file.close();

    if (csv_file.fail())
    {
        // Put a breakpoint here: CSV write or close failed.
        while (true)
        {
        }
    }
    // Put a breakpoint here: all samples have been saved.
    while (true)
    {
        __WFI();
    }
    //End

    return 0;
}

double baro_altitude(Board& hw)
{
    const double baro_measure = hw.bmp390.get_pressure();
    return cal_altitude(baro_measure, ATMOSPH_PRESS_InHg);
}

AvgBias param_average(Board& hw)
{
    // Keep the board stationary throughout calibration.
    for (uint16_t n = 0; n < MAX_CALIB_SIZE; ++n)
    {
        hw.bno055.read_all(reading);
        altitude = baro_altitude(hw);

        const double qw = reading.quat.w;
        const double qx = reading.quat.x;
        const double qy = reading.quat.y;
        const double qz = reading.quat.z;

        const double ax = reading.linear_accel.x;
        const double ay = reading.linear_accel.y;
        const double az = reading.linear_accel.z;

        const double az_rot =
            2 * ax * (qx * qz - qw * qy) +
            2 * ay * (qy * qz + qw * qx) +
            az * (1 - 2 * (qx * qx + qy * qy));

        samples[n].z_meas(1) = az_rot;
        samples[n].z_meas(0) = altitude;

        HAL_Delay(20);
    }

    double accel_bias = 0.0;
    double baro_bias = 0.0;

    for (uint16_t n = 0; n < MAX_CALIB_SIZE; ++n)
    {
        accel_bias += samples[n].z_meas(1);
        baro_bias += samples[n].z_meas(0);
        
    }

    return {(accel_bias / MAX_CALIB_SIZE), (baro_bias / MAX_CALIB_SIZE)};
}