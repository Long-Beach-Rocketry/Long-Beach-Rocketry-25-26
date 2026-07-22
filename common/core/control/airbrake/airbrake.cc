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

    // Altitude units is in meters
    float altitude = calc_altitude(init_pressure, pressure) - base_altitude;
    float time;          // temp placeholder for time from start
    bool launch_signal;  // temp placeholder

    if (altitude < 0 || AirbrakeConstants::kTargetApogee < altitude)
    {
        /**
         * Either it's negative and the altitude calculation is bugging or
         * we hit the target apogee and should recover?
         * 
         * Which state? AIRBRAKES_RETRACTING, AIRBRAKES_FULLY_RETRACTED, RECOVERY_POPPED
         */
        motor.set_angle(0);
        state = AirbrakeState::RECOVERY_POPPED;
    }

    switch (state)
    {
        case AirbrakeState::PRELAUNCH:

            if (launch_signal)
            {
                state = AirbrakeState::LAUNCHED;
            }
            else
            {
                init_pressure = data.pressure;
                base_altitude = calc_altitude(init_pressure, data.pressure);
            }
            break;

        case AirbrakeState::LAUNCHED:

            /**
             * Check if altitude is at motor burn altitude or 6 seconds passed
             */
            if (time >= AirbrakeConstants::kMotorBurnTime ||
                altitude >= AirbrakeConstants::kMotorBurnAlt)
            {
                state = AirbrakeState::AIRBRAKES_DEPLOYING;
            }
            break;

        case AirbrakeState::AIRBRAKES_DEPLOYING:
            motor.set_angle(AirbrakeConstants::kMaxAngle);

            // Not sure how to transition to fully deployed state. Below is pseudocode
            float current_angle = motor.get_angle();
            if (current_angle >= AirbrakeConstants::kMaxAngle)
            {
                state = AirbrakeState::AIRBRAKES_FULLY_DEPLOYED;
            }
            break;

        case AirbrakeState::AIRBRAKES_FULLY_DEPLOYED:
            if (altitude >=
                AirbrakeConstants::kTargetApogee /* OR VELOCITY close to 0 */)
            {
                state = AirbrakeState::AIRBRAKES_RETRACTING;
            }
            break;

        case AirbrakeState::AIRBRAKES_RETRACTING:
            motor.set_angle(AirbrakeConstants::kRetractAngle);
            // Not sure how to transition to fully retracted state. Below is pseudocode
            float current_angle = motor.get_angle();
            if (current_angle <= 0)
            {
                state = AirbrakeState::AIRBRAKES_FULLY_RETRACTED;
            }
            break;

        case AirbrakeState::AIRBRAKES_FULLY_RETRACTED:
            // Not sure what to do here
            /**
             * Maybe if acceleration or velocity downward, transition state
             * 
             * Or if receive signal for recovery popped but this not needed if so
             */
            state = AirbrakeState::RECOVERY_POPPED;
            break;

        case AirbrakeState::RECOVERY_POPPED:
            break;
        default:
            break;
    }
}

}  // namespace LBR