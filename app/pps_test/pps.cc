#include "pps.h"
#include "pps_hw.h"
#include <cmath>

namespace LBR {

Pps::Pps() = default;

PpsState Pps::getState() const {
    return state_;
}

void Pps::update(const LBR::Bno055Data& imu_data) {
    // Update state_quat_ from IMU data
    state_quat_[0] = imu_data.quat.w;
    state_quat_[1] = imu_data.quat.x;
    state_quat_[2] = imu_data.quat.y;
    state_quat_[3] = imu_data.quat.z;

    // Update limit switches from GPIO
    limit_switch_max = LBR::lms(); // Use the correct function for limit switch

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
            // Deploy: move mechanism to deployed position (limit_switch_max)
            motorDeploy();
            if (limit_switch_max) {
                motorStopAll();
                state_ = PpsState::Rotate;
            }
            break;
        case PpsState::Rotate:
            // Rotate: move to dig position (could be a specific angle or position)
            motorTarget();
            if (atDigPosition()) {
                 motorStopAll();
                 state_ = PpsState::FlipAuger;
            }
            break;
        case PpsState::FlipAuger:
            // Flip auger to start drill
            if (flipDone()) {
                state_ = PpsState::Drill;
            }
            break;
        case PpsState::Drill:
            // Drill: perform drilling operation
            // servoMotorDrill();
            if (drillDone()) {
                state_ = PpsState::SampleRead;
            }
            break;
        case PpsState::SampleRead:
            // SampleRead: read sample sensor
            // soilSampleRead();
            if (sampleOk()) {
                state_ = PpsState::Done;
            }
            break;
        case PpsState::Done:
            break;
    }
}

// Private pps state helper implementations

bool Pps::readLms() {
    return lms();
}

bool LBR::Pps::shouldDeploy() {
    if (!LBR::lms()) return false;
    for (int i = 0; i < 4; ++i) {
        if (state_quat_[i] != 0.0f) return true;
    }
    return false;
}

bool LBR::Pps::atDigPosition() {
    // Come with testing prototype
    // Replace DIG_TARGET_QUAT with actual target quaternion values
    constexpr float DIG_TARGET_QUAT[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    constexpr float QUAT_TOLERANCE = 0.05f; // Acceptable error margin
    for (int i = 0; i < 4; i++) {
        if (std::abs(state_quat_[i] - DIG_TARGET_QUAT[i]) > QUAT_TOLERANCE) {
            return false;
        }
    }
    return true;
}

bool LBR::Pps::flipDone() {
    // Check if the IMU quaternion indicates the auger is flipped to the target orientation
    // Replace FLIP_TARGET_QUAT with actual target quaternion values for the flipped position
    constexpr float FLIP_TARGET_QUAT[4] = {0.7071f, 0.7071f, 0.0f, 0.0f}; // Example: 90-degree rotation about X
    constexpr float QUAT_TOLERANCE = 0.05f; // Acceptable error margin
    for (int i = 0; i < 4; i++) {
        if (std::abs(state_quat_[i] - FLIP_TARGET_QUAT[i]) > QUAT_TOLERANCE) {
            return false;
        }
    }
    return true;
}

bool LBR::Pps::drillDone() {
    // TODO: Implement logic
    // Based on the servo motor status or drilling time
    return false;
}

bool LBR::Pps::sampleOk() {
    // TODO: Implement logic
    // Based on the sample sensor reading
    return false;
}

} // namespace LBR
