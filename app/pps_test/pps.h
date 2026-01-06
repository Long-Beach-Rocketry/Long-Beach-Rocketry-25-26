/**
 * @file pps.h
 * @brief PPS module interface
 * @note This module manages the PPS state machine for deployment, rotation, drilling, and sample read.
 * @author Bex Saw
 * @date 2026/01/05
 */

#pragma once
#include "bsp/motor_if.h" // Use generic motor interface for portability
// #include "bno055_imu.h" // Uncomment if IMU is used

namespace LBR {

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
public:
    Pps();
    void update();
    PpsState getState() const;

private:
    // IMU data fields (quaternion, etc.)
    // ...
    // PWM control fields
    // ...

    // Limit switches: min = stowed/home position, max = fully deployed position
    bool limit_switch_min_ = false; // True when mechanism is at home/stowed position
    bool limit_switch_max_ = false; // True when mechanism is at deployed/limit position

    PpsState state_;

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
     * @return true if drilling is done, false otherwise.
     */
    bool drillDone();

    /**
     * @brief Check if sample sensor indicates OK.
     * @return true if sample is OK, false otherwise.
     */
    bool sampleOk();
};

} // namespace LBR
        
