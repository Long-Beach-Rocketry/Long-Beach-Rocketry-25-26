#include "airbrake.h"
#include <cstdint>
#include "airbrake_constants.h"
#include "airbrake_utils.h"

namespace LBR
{

AirBrake::AirBrake(AirbrakeParams params_)
    : motor{params_.motor}, state{AirbrakeState::PRELAUNCH}, base_altitude{0}
{
}

AirbrakeState AirBrake::get_state() const
{
    return state;
}

void AirBrake::update(SensorData data)
{
    /**
     * INPUT FOR :
     * TARGET_TELEM
     * LAUNCH_SIGNAL
     */

    /* TODO: replace these with actual values */
    // Altitude units is in meters
    float altitude = calc_altitude(init_pressure, pressure) - base_altitude;
    float velocity;      // temp placeholder for velocity
    float time;          // temp placeholder for time from start
    bool launch_signal;  // temp placeholder

    if (altitude < 0 || AirbrakeConstants::kTargetApogee < altitude)
    {
        /**
         * Either it's negative and the altitude calculation is bugging or
         * we hit the target apogee and should recover?
         * 
         * Which state? AIRBRAKES_RETRACTING, AIRBRAKES_RETRACTED, RECOVERY_POPPED
         */
        motor.set_angle(0);
        state = AirbrakeState::RECOVERY_POPPED;
    }

    switch (state)
    {
        case AirbrakeState::PRELAUNCH:

            if (launch_signal)
            {
                state = AirbrakeState::MOTOR_BURN;
            }
            else
            {
                init_pressure = data.pressure;
                base_altitude = calc_altitude(init_pressure, data.pressure);
            }
            break;

        case AirbrakeState::MOTOR_BURN:

            /**
             * Check if altitude is at motor burn altitude or 6 seconds passed
             */
            if (time >= AirbrakeConstants::kMotorBurnTime ||
                altitude >= AirbrakeConstants::kMotorBurnAlt)
            {
                state = AirbrakeState::AIRBRAKES_DEPLOYED;
            }
            break;

        case AirbrakeState::AIRBRAKES_DEPLOYED:

            // Not sure how to transition to retracting state. What triggers it to even do it?
            /* TODO: Get angle from PMC and set the servo motor angle */
            float angle = mpc_get_angle();

            if (angle == 0)
            {
                state = AirbrakeState::AIRBRAKES_RETRACTING;
            }
            else
            {
                motor.set_angle(angle);
            }
            break;

        case AirbrakeState::AIRBRAKES_RETRACTING:
            motor.set_angle(AirbrakeConstants::kRetractAngle);
            // Not sure how to transition to fully retracted state. Below is pseudocode
            float current_angle = motor.get_angle();
            if (current_angle <= 0)
            {
                state = AirbrakeState::AIRBRAKES_RETRACTED;
            }
            break;

        case AirbrakeState::AIRBRAKES_RETRACTED:
            // Not sure what to do here

            if (altitude >= AirbrakeConstants::kTargetApogee && velocity == 0)
            {
                state = AirbrakeState::RECOVERY_POPPED;
            }
            break;

        case AirbrakeState::RECOVERY_POPPED:
            break;
        default:
            break;
    }
}

}  // namespace LBR