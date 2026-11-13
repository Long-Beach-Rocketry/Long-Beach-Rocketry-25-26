//#include <Eigen/Dense>
#include <cmath>
#include <cstdlib>
#include <fstream>  // For CSV file stream logging operations
#include <iomanip>
#include "baro_math.h"
#include "board.h"
#include "kalman.h"
#include "stm32h7xx_hal.h"  // For access to the global clock function HAL_Delay()

using namespace Eigen;
using namespace std;
using namespace LBR;

static constexpr double ATMOSPH_PRESS_InHg{29.98};
static constexpr uint16_t MAX_SAMPLING_SIZE{500};
static constexpr uint8_t MAX_CALIB_SIZE{60};

double baro_altitude(Board& hw);
AverageBias param_average(Board& hw);
EKF filter_init();
void csv_processing();

Info samples[MAX_SAMPLING_SIZE];
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

    const AverageBias param_bias = param_average(hw);
    double accel_offset = param_bias.accel_bias;
    double altitude_offset = param_bias.altitude_bias;

    EKF ekFilter = filter_init();

    uint16_t sample_count = 0;

    double ax, ay, az, qw, qx, qy, qz;
    uint32_t previous_sample_ms = HAL_GetTick();

    while (sample_count < MAX_SAMPLING_SIZE)
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
        double az_rot =
            (2 * ax * (qx * qz - qw * qy) + 2 * ay * (qy * qz + qw * qx) +
             az * (1 - 2 * (pow(qx, 2) + pow(qy, 2)))) -
            accel_offset;

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
        z(1) = az_rot;

        ekFilter.predict(dt_seconds);
        ekFilter.update(z);

        // Keep this global variable available in Watch.
        kf_state = ekFilter.get_state();

        Info& record = samples[sample_count];

        record.time_ms = sample_time_ms;
        record.z_meas = z;
        record.x_state = kf_state;
        ++sample_count;
    }

    csv_processing();

    return 0;
}

EKF filter_init()
{
    KalmanMatrix model;
    model.F.setIdentity();

    model.H << 1, 0, 0, 0, 0, 1;

    const double alpha = 0.02;

    model.Q.setZero();
    model.Q.diagonal() << 7.1e-3 * pow(alpha, 4), 1.35e-4 * pow(alpha, 2),
        14e-2 * alpha;

    model.R.setZero();
    model.R.diagonal() << 0.2, 0.03;

    EKF filter{model};

    KalmanParams params;
    params.x.setZero();
    params.x_predict = params.x;

    params.P.setZero();
    params.P.diagonal() << 3.0, 1, 0.002;
    params.P_predict = params.P;

    filter.init(params);
    return filter;
}

double baro_altitude(Board& hw)
{
    const double baro_measure = hw.bmp390.get_pressure();
    return cal_altitude(baro_measure, ATMOSPH_PRESS_InHg);
}

AverageBias param_average(Board& hw)
{
    // Keep the board stationary throughout calibration.
    Info samples[MAX_CALIB_SIZE];
    for (uint8_t n = 0; n < MAX_CALIB_SIZE; ++n)
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

        const double az_rot = 2 * ax * (qx * qz - qw * qy) +
                              2 * ay * (qy * qz + qw * qx) +
                              az * (1 - 2 * (qx * qx + qy * qy));

        samples[n].z_meas(1) = az_rot;
        samples[n].z_meas(0) = altitude;

        HAL_Delay(20);
    }

    double accel_bias = 0.0;
    double baro_bias = 0.0;

    for (uint8_t n = 0; n < MAX_CALIB_SIZE; ++n)
    {
        accel_bias += samples[n].z_meas(1);
        baro_bias += samples[n].z_meas(0);
    }

    return {(accel_bias / MAX_CALIB_SIZE), (baro_bias / MAX_CALIB_SIZE)};
}

void csv_processing()
{
    //Process the sample array to store it into the csv file
    ofstream csv_file("flight_data.csv", ios::out | ios::trunc);

    if (!csv_file.is_open())
    {
        // Pause here and inspect csv_open_errno.
        while (true)
        {
        }
    }

    csv_file << setprecision(15);

    csv_file << "time_ms,z_pz,z_az,kf_pz,kf_vz,kf_az\n";

    for (uint16_t n = 0; n < MAX_SAMPLING_SIZE; ++n)
    {
        const Info& record = samples[n];

        csv_file << record.time_ms << ',' << record.z_meas(0) << ','
                 << record.z_meas(1) << ',' << record.x_state(0) << ','
                 << record.x_state(1) << ',' << record.x_state(2) << '\n';

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
}