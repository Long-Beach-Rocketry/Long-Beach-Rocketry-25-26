#include "airbrake.h"
#include <cstdint>
#include "airbrake_constants.h"

namespace LBR
{

AirBrake::AirBrake(AirbrakeParams params_)
    : motor{params_.motor}, state{AirbrakeState::PRELAUNCH}
{
}

bool AirBrake::init()
{
    /**
   * Maybe we could use the barometer to calculate altitude to set base_altitude.
   * 
   */
    return true;
}

AirbrakeState AirBrake::getState() const
{
    return state;
}

void AirBrake::fetchImuData(const LBR::Bno055Data& data)
{
    imu = data;
}

void AirBrake::fetchPressData(const float data)
{
    pressure = data;
}

void AirBrake::fetchTempData(const float data)
{
    temperature = data;
}

void AirBrake::update()
{
    /**
   * INPUT FOR :
   * TARGET_TELEM
   * LAUNCH_SIGNAL
   */

    float altitude = calcAltitude() - base_altitude;
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