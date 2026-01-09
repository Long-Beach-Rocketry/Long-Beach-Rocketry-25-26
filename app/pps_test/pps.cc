
#include "pps.h"

namespace LBR {

Pps::Pps() {
    state_ = PpsState::Idle; // Only set initial state, do not init hardware here
}

void Pps::update() {
    // Update limit switches from GPIO
    limit_switch_min_ = readLmsMin();
    limit_switch_max_ = readLmsMax();

    switch (state_) {
        case PpsState::Start:
            // Initial state: prepare system, wait for Idle
            state_ = PpsState::Idle;
            break;
        case PpsState::Idle:
            // Idle: waiting for start sequence command
            if (shouldDeploy()) state_ = PpsState::Deploy;
            break;
        case PpsState::Deploy:
            // Deploy: move mechanism to deployed position (limit_switch_max_)
            // PWM: drive motor to extend
            if (limit_switch_max_) state_ = PpsState::Rotate;
            break;
        case PpsState::Rotate:
            // Rotate: move to dig position (could be a specific angle or position)
            // PWM: rotate mechanism
            // TODO: check if at dig position
            // if (atDigPosition())
            state_ = PpsState::FlipAuger;
            break;
        case PpsState::FlipAuger:
            // Flip auger to start drill
            // PWM: actuate auger
            // TODO: check if flip done
            // if (flipDone())
            state_ = PpsState::Drill;
            break;
        case PpsState::Drill:
            // Drill: perform drilling operation
            // PWM: run drill motor
            // TODO: check if drill done
            // if (drillDone())
            state_ = PpsState::SampleRead;
            break;
        case PpsState::SampleRead:
            // SampleRead: read sample sensor
            // TODO: check if sample OK
            // if (sampleOk())
            state_ = PpsState::Done;
            break;
        case PpsState::Done:
            // Done: sequence complete
            // Optionally reset or return to Idle state
            break;
    }
}

PpsState Pps::getState() const {
    return state_;
}

} // namespace LBR
