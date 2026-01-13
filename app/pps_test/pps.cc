#include "pps.h"
#include <cmath>

namespace LBR {

Pps::Pps() = default;

PpsState Pps::getState() const {
    return state_;
}

void Pps::fetchImuData(const LBR::Bno055Data& imu_data) {
    // Update state_quat_ from IMU data
    state_quat_[0] = imu_data.quat.w;
    state_quat_[1] = imu_data.quat.x;
    state_quat_[2] = imu_data.quat.y;
    state_quat_[3] = imu_data.quat.z;
}

void Pps::update() {
    // Update limit switches from GPIO
    limit_switch_max = readLimitSwitchMax();
    limit_switch_min = readLimitSwitchMin();    

    switch (state_) {
        case PpsState::Start:
            state_ = PpsState::Idle;
            break;
        case PpsState::Idle:
            // Idle: waiting for start sequence command
            if (deploy()) state_ = PpsState::Deploy;
            break;
        case PpsState::Deploy:
            // Deploy: move mechanism to deployed position (limit_switch_max)
            motorDeploy();
            if (limit_switch_max) {
                motorStop();
                state_ = PpsState::Position;
            }
            break;
        case PpsState::Position:
            // Position: move to target position
            motorTarget();
            // Transition to Retract if retraction condition met 
            if (limit_switch_min && state_ != PpsState::Retract && state_ != PpsState::Done) {
                motorStop();
                state_ = PpsState::Retract;
            }
            break;
        case PpsState::Retract:
            // Retract mechanism
            motorRetract();
            if (limit_switch_min) {
                motorStop();
                state_ = PpsState::Done;
            }
            break;
        case PpsState::Done:
            break;
    }
}

bool Pps::readLimitSwitchMax() {
    return (GPIOA->IDR & (1 << 3)) != 0;
}

bool Pps::readLimitSwitchMin() {
    return (GPIOA->IDR & (1 << 3)) != 0;
}
bool Pps::deploy() {
    if (!readLimitSwitchMax()) return false;
    for (int i = 0; i < 4; ++i) {
        if (state_quat_[i] != 0.0f) return true;
    }
    return false;
}

bool Pps::retracted() {
    return readLimitSwitchMin();
}

} // namespace LBR
