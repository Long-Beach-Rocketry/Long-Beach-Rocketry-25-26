#include "pps.h"

void ppsInit(PpsContext& ctx) {
    ctx.state = PpsState::Idle;
}

void ppsUpdate(PpsContext& ctx) {
    // TODO: Read IMU, GPIO, run state machine, call motor_if
    switch (ctx.state) {
        case PpsState::Idle:
            // ...
            break;
        case PpsState::Homing:
            // ...
            break;
        case PpsState::MovingToTarget:
            // ...
            break;
        case PpsState::AtEndstop:
            // ...
            break;
        case PpsState::Fault:
            // ...
            break;
    }
}
