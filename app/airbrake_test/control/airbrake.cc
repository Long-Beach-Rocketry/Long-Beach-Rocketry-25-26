#include "airbrake.h"
#include <cstdint>

namespace LBR
{

/**
 * Placeholder altitudes (should all be precalculated)
 *            .
 *           /  Target Apogee
 *          /
 *         /    Target 2
 *        /     
 *       /      Target 1
 * .____/
 * 
 */
constexpr float kTargetAlt1;
constexpr float kTargetAlt2;
constexpr float kTargetAlt3; 
/* Target Apogee ^ suppoed to be pre-calculated or from CAN??? */


/**
 * Not sure what the angles gonna be yet either
 * 
 *      .____________  
 *      |\/
 *      |~\
 *      |  \
 *      |   \   
 *      |    \
 * 
 */
constexpr uint8_t kTargetAng1;
constexpr uint8_t kTargetAng2;
constexpr uint8_t kTargetAng3;

AirbrakeState AirBrake::getState() const {
  return state;
}

void AirBrake::fetchImuData(const LBR::Bno055Data& data) {
  imu = data;
}

void AirBrake::fetchPressData(const float data) {
  pressure = data;
}  

void AirBrake::fetchTempData(const float data) {
  temperature = data;
}  

float AirBrake::calcAltitude() const {
  /**
   * Formula depends on weather and pressure of the day
   * apparently.
   * 
   * Also something with temperature not really used if 
   * not changing?
   * 
   * Not sure calculating absolute or relative altitude.
   * 
   */
}

void AirBrake::update() {
  /**
   * Get CAN INPUT FOR :
   * TARGET_TELEM
   * LAUNCH_SIGNAL
   */

  float altitude = calcAltitude();
  bool launch_signal; // temp placeholder

  switch (state)
  {
  case AirbrakeState::WAITING:
    /* Get CAN input and check if ascent */
    if (launch_signal /* LAUNCH SIGNAL */ ) {
      state = AirbrakeState::ASCENT;
    }
    else { break; }

  case AirbrakeState::ASCENT:
    /* Set pwm to idk when at apogee */
    /**
     * Servo motor takes a uint16_t angle arg with `set_angle`
     * so need to do anything with PWM?
     */

    // uint8_t be compared with floats
    if (kTargetAlt1 <= altitude && altitude < kTargetAlt2) {
      motor.set_angle(kTargetAng1);
    }
    else if (kTargetAlt2 <= altitude && altitude < kTargetAlt3) {
      motor.set_angle(kTargetAng2);
    }
    else if (kTargetAlt3 <= altitude) {
      state = AirbrakeState::APOGEE;
    }
    break;
    
  case AirbrakeState::APOGEE:
    /** 
     * Placing this here will repeatedly set angle to 90 (or whatever val).
     * Not sure if motor will preserve the angle automatically or not.
     * 
     * or if this is wasteful and unnecessary.
     */
    motor.set_angle(90); 
    break;
  default:
    break;
  }

  /* 
    Do we still need to calculate current velocity to compare with
    target velocity?
  */

  /**
   * Send CAN OUTPUT FOR :
   * Angle
   * Health?
   */
}


}