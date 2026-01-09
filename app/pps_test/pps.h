/**
 * @file pps.h
 * @brief PPS module interface
 * @note This module manages the PPS state machine for deployment, rotation, drilling, and sample read.
 * @author Bex Saw
 * @date 2026/01/05
 */

#pragma once
#include "bsp/motor_if.h" 
#include "bno055_imu.h" 

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
public:
    Pps();
    void update();
    PpsState getState() const;

private:
    PpsState state_;

    struct ImuData {
    // Quaternion representing orientation (w, x, y, z)
        float quat[4];
    };
    
    // Reads the quaternion from the BNO055 IMU into the provided struct
    void read_imu(ImuData* data);

    // PWM control fields
    // TODO: Define PWM control methods and members as needed

    // Limit switch states
    static bool readLmsMin(); // Stowed switch
    static bool readLmsMax(); // Deployed switch
    bool limit_switch_min_ = false;
    bool limit_switch_max_ = false; 

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


