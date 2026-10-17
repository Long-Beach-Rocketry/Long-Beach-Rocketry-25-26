#include "airbrake.h"
#include "board.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    board_init();
    Board& hw = get_board();

    // Initialize kalman filter

    // Initialize mpc

    // Initialize servo motor

    // Initialize sensor manager

    AirbrakeParams params{hw.sensor_mgr, hw.motor};

    Airbrake airbrake(airbrake_params);
    airbrake.init();

    while (1)
    {
        // hw.imu.read_all(data);
        // airbrake.fetch_imu(data);

        // press = hw.bmp390.get_pressure();
        // temp = hw.bmp390.get_temperature();

        // airbrake.fetch_press(press);
        // airbrake.fetch_temp(temp);

        airbrake.update();
    }

    return 0;
}