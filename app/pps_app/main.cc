#include "pps.h"
#include "board.h"

using LBR::Pps;

Board board;

int main() {
    Pps pps;
    while (true) {
        // Simulate fetching IMU data
        pps.fetchImuData(board.imu);
        pps.fetchAccelData(board.accel);
        // Update PPS state machine
        pps.update();
    }
    return 0;
}