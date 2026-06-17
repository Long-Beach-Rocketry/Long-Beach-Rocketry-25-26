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

constexpr float kTempLapseRate{.0065};  // Kelvin / meters
constexpr float kGravity{9.80655};      // meters / sec ^ 2
constexpr float kAirMolarMass{.0289};   // kg / mol
constexpr float kUnivGasConst{8.314};   // J / (mol * K)

constexpr float kMotorBurnTime{6};     // In seconds
constexpr float kMotorBurnAlt{990.6};  // In ft
constexpr float kTargetApogee{2286};   // In meters
constexpr int kRetractAngle{0};        // In degrees
constexpr int kMaxAngle{30};           // In degrees

}  // namespace AirbrakeConstants

}  // namespace LBR