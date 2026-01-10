#pragma once
/**
 * @file pps.h
 * @brief PPS module interface
 * @note This module manages the PPS state machine for deployment, rotation, drilling, and sample read.
 * @author Bex Saw
 * @date 2026/01/05
 */
#include "bsp_pps/motor_if.h"
#include "bno055_imu.h" // For Bno055Data
#include "pps_hw.h" // For limit switch abstraction

namespace LBR {

/**
 * @enum PpsState
 * @brief States of the PPS state machine.
 */
enum class PpsState {
    Start,        // Initial state
    Idle,         // Waiting for command
    Deploy,       // Deploy to limit switch
    Rotate,       // Rotate to dig position
    FlipAuger,    // Flip auger to start drill
    Drill,        // Drilling operation
    SampleRead,   // Read sample sensor
    Done          // Sequence complete
};

class Pps {
    float state_quat_[4] = {0};
public:
    PpsState getState() const;
    Pps();
    void update(const LBR::Bno055Data& imu_data); // Only one update, takes IMU data

private:
    PpsState state_ = PpsState::Idle;

    bool limit_switch_max = false;

    /**
     * @brief Read the current state of the limit switch (PA3).
     * @return true if the switch is active, false otherwise.
     */
    static bool readLms();

    // Motor control helpers
    void motorDeploy() { motorDeployDir(); }
    void motorStopAll() { motorStop(); }
    void motorTarget() { motorTargetDir(); }

    /**
     * @brief Check if deploy sequence should start.
     * @return true if deployment should begin, false otherwise.
     * @note Used to determine when to transition from Idle to Deploy state.
     */
    bool shouldDeploy();

    /**
     * @brief Check if mechanism is at dig position.
     * @return true if at dig position, false otherwise.
     */
    bool atDigPosition();

    /**
     * @brief Check if auger flip is complete.
     * @return true if flip is done, false otherwise.
     */
    bool flipDone();

    /**
     * @brief Check if drilling operation is complete.
     * @return true if servo motor drilling is done, false otherwise.
     */
    bool drillDone();

    /**
     * @brief Check if sample sensor indicates OK.
     * @return true if sample is OK, false otherwise.
     */
    bool sampleOk();
};

} // namespace LBR
