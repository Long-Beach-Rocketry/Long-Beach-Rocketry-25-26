/**
 * @file airbrake_constants.h
 * @brief Airbrake constants used by the airbrake app
 * @note This file uses information from the Airbrake ConOps doc and the simulation graph 
*/

#pragma once

namespace LBR
{

namespace AirbrakeConstants
{
constexpr int kMotorBurnTime = 6;      // In seconds
constexpr float kMotorBurnAlt = 3250;  // in ft
constexpr float kTargetApogee = 7500;  // in ft
constexpr int kRetractAngle = 0;       // In degrees
constexpr int kMaxAngle = 30;          // In degrees

}  // namespace AirbrakeConstants

}  // namespace LBR