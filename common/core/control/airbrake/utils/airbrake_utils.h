/**
 * @file airbrake_utils.h
 * @brief Helper functions for data used by the airbrake
 * @author Joseph Chang
*/

#pragma once
#include "bmp390.h"
#include "bno055_imu.h"

/**
 * @brief Helper function to convert Celsius temperature to Kelvins
 * @param temp The temperature in Celsius to convert
 * @return The same temperature but in Kelvin units
 */
inline float celsius_to_kelvin(float temp);

/**
 * @brief Helper function to calculate altitude from barometer information
 * @note Using the following formula:
 * altitude  = T_0 / L * [1 - (P / P_0)^(R * L / g / M)]
 * where T_0 = Initial temperature (celsius)
 *       L   = ISA Temperature Lapse Rate (K / m)
 *       P   = pressure (hPa)
 *       P_0 = Initial pressure (hPa)
 *       R   = Universal gas constant (J / (mol * K))
 *       g   = gravity acceleration constant (m / s^2)
 *       M   = Molar mass for dry air (kg / mol)
 *       
 * @param init_temp The initial temperature read by the barometer (in celsius)
 * @param init_press The initial pressure read by the barometer
 * @param curr_press The current pressure reading by the barometer
 * @return The calculated altitude in meters
 */
float calc_altitude(float init_temp, float init_press, float curr_press);

/**
 * @brief Helper function that calculates the current velocity
 * @note this function aint it. Will be changed in future
 * Velocity = V0 + aZ * time
 * @param v0 The last read velocity (m / s)
 * @param aZ Current acceleration (m / s^2)
 * @param time Time from last reading
 * @return The current velocity of the rocket
 */
float calc_velocity(float v0, float aZ, float time);

/**
* @brief Helper function to check the imu sensor health at start of main
 * @param bno055 The imu sensor used
 * @return True if successful and healthy, False otherwise
 */
bool monitor_imu(Bno055& bno055);

/**
 * @brief Helper function to check the barometer sensor health at start of main
 * @param bmp390 The barometer sensor used
 * @return True if successful and healthy, False otherwise
 */
bool monitor_baro(Bmp390& bmp390);