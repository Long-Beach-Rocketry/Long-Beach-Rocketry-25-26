#include "airbrake.h"
#include "airbrake_utils.h"
#include "bmp390.h"
#include "bno055_imu.h"

Bno055Data data;
float press, temp;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    // should probably pass this in the airbrake ye?
    monitor_imu(hw.imu);
    monitor_baro(hw.bmp390);

    press = hw.bmp390.get_pressure();

    Airbrake airbrake(motor);

    while (1)
    {
        hw.imu.read_all(data);
        airbrake.fetch_imu(data);

        press = hw.bmp390.get_pressure();
        temp = hw.bmp390.get_temperature();

        airbrake.fetch_press(press);
        airbrake.fetch_temp(temp);

        airbrake.update();
    }

    return 0;
}