#pragma once
/**
 * @file pps.h
 * @brief PPS module interface
 * @note This module manages the PPS state machine for deployment, rotation, drilling, and sample read.
 * @author Bex Saw
 * @date 2026/01/05
 */
#include "motor_support/motor_if.h"
#include "bno055_imu.h" // For (quaternion)
#include "stm32l476xx.h"

namespace LBR {

/**
 * @enum PpsState
 * @brief States of the PPS state machine.
 */
enum class PpsState {
    Start,        // Initial state
    Idle,         // Waiting for command
    Deploy,       // Deploy to limit switch
    Position,     // Move to position
    Retract,      // Retract mechanism
    Done          // Sequence complete
};

class Pps {
    float state_quat_[4] = {0};
public:
    PpsState getState() const;
    Pps();
    void fetchImuData(const LBR::Bno055Data& imu_data); // Update IMU data only
    void update(); // State machine update, no IMU arg

private:
    PpsState state_ = PpsState::Idle;

     bool limit_switch_max = false; // Deployed position
     bool limit_switch_min = false; // Retracted position

    /**
     * @brief Read the current state of the limit switch (PA3).
     * @return true if the switch is active, false otherwise.
     */
     static bool readLimitSwitchMax();

     /**
      * @brief Read the current state of the min limit switch (e.g. PA4, retracted).
      * @return true if the switch is active, false otherwise.
      */
     static bool readLimitSwitchMin();
    /**
     * @brief Check if deploy sequence should start.
     * @return true if deployment should begin, false otherwise.
     * @note Used to determine when to transition from Idle to Deploy state.
     */
    bool deploy();

    /**
     * @brief Check if mechanism is retracted. 
     * @return true if retracted, false otherwise.
     * @note Used to determine when to transition from Retract to Done state.
     */
    bool retracted();
};

} // namespace LBR
