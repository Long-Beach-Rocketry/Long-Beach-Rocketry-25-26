/**
 * @file airbrake_utils.h
 * @brief Helper functions for data used by the airbrake
 * @author Joseph Chang
*/

/**
 * @brief Helper function to convert Celsius temperature to Kelvins
 * @param temp The temperature in Celsius to convert
 * @return The same temperature but in Kelvin units
 */
inline float celsiusToKelvin(float temp);

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
float calcAltitude(float init_temp, float init_press, float curr_press);

/**
 * @brief Helper function that calculates the current velocity
 * @note this function aint it. Will be changed in future
 * Velocity = V0 + aZ * time
 * @param v0 The last read velocity (m / s)
 * @param aZ Current acceleration (m / s^2)
 * @param time Time from last reading
 * @return The current velocity of the rocket
 */
float calcVelocity(float v0, float aZ, float time);