#include "pps.h"

namespace LBR {

Pps::Pps() = default;

PpsState Pps::getState() const {
    return state_;
}

void Pps::fetchImuData(const LBR::Quaternion& data) {
    // Store quaternion data from IMU
    state_quat_ = data;
}

void Pps::fetchAccelData(const LBR::Vec3& data) {
    // Store acceleration data from IMU
    state_accel_ = data;
}

void Pps::update() {    

    switch (state_) {
        case PpsState::Idle:
            // Idle: waiting for Deploy or Retract command
            if (deploy()) {
                state_ = PpsState::Deploying;
            } else if (retracted()) {
                state_ = PpsState::Retract;
            }
            break;
        case PpsState::Deploying:
            // Deploying: move to deployed position, then rotate, then return to Idle
            motorDeploy();
            if (readLimitSwitch() == LimitSwitchState::extended) {
                motorStop();
                // After deploying, rotate
                state_ = PpsState::Rotating;
            }
            break;
        case PpsState::Rotating:
            // Rotating: move to target/drill position, then return to Idle
            motorTarget();
            if (rotationComplete()) {
                motorStop();
                state_ = PpsState::Idle;
            }
            break;
        case PpsState::Retract:
            // Retract: move to retracted position, then return to Idle
            motorRetract();
            if (readLimitSwitch() == LimitSwitchState::retracted) {
                motorStop();
                state_ = PpsState::Idle;
            }
            break;
    }
}

bool Pps::readLimitSwitch() {
    // Read limit switch state from GPIO
    if (gpio_.readPin(LIMIT_SWITCH_PIN)) {
        return LimitSwitchState::extended;
    } else {
        return LimitSwitchState::retracted;
    }
}


bool Pps::deploy() {
    if (!(readLimitSwitch() == LimitSwitchState::extended)) return false;
    for (int i = 0; i < 4; ++i) {
        if (state_quat_[i] != 0.0f) return true;
    }
    return false;
}

bool Pps::retracted() {
    // Use limit switch and IMU acceleration to determine if retracted
    static LBR::Vec3 prev_accel = {0, 0, 0};
    LBR::Vec3 accel = state_accel_; 

    if (readLimitSwitch() == LimitSwitchState::retracted) {
        // Check if mechanism is moving (acceleration changed)
        float delta = (accel.x - prev_accel.x) * (accel.x - prev_accel.x)
                   + (accel.y - prev_accel.y) * (accel.y - prev_accel.y)
                   + (accel.z - prev_accel.z) * (accel.z - prev_accel.z);
        prev_accel = accel;
        // If not moving (delta is small), we are truly retracted
        if (delta < 0.01f) {
            return true;
        }
    }
    prev_accel = state_accel_;
    return false;
}  

bool Pps::rotationComplete() {
    // Placeholder logic: In real implementation, use encoder or IMU feedback
    static int rotation_steps = 0;
    const int TARGET_ROTATION_STEPS = 100; // Random threshold number (come with testing)
    rotation_steps++;
    if (rotation_steps >= TARGET_ROTATION_STEPS) {
        rotation_steps = 0; // Reset for next rotation
        return true;
    }
    return false;
}
} // namespace LBR
