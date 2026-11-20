/** 
* @file atmosphere.h
* @author Mario Cruz
* @brief International Standard Atmosphere (ISA) troposphere model
* @date 9-30-2026
*/

#pragma once

namespace LBR
{

/**
 * @brief Pressure and temperature measured at a given altitude
 */
struct AtmosphereConditions
{
    double pressure_pa;    // Pascal
    double temperature_k;  // Kelvin
};

class Atmosphere
{
public:
    /**
     * @brief Creates the atmosphere model for one launch
     * @param launch_conditions_ Measured pressure and temperature at launch site. Every calculation starts from here
     */
    explicit Atmosphere(const AtmosphereConditions& launch_conditions_);

    /**
     * @brief Standard conditions at a launch site, used as launch conditions until real ones are measured
     * @param elevation_m Launch pad elevation above sea level
     * @param launch_conditions_out Filled with the site's standard conditions
     * @return true if successful, false if elevation_m isn't valid
     */
    static bool standard_atmosphere(
        double elevation_m, AtmosphereConditions& launch_conditions_out);

    /**
     * @brief Safety checks that conditions are physically possible
     * @return true if pressure and temperature are positive real numbers
     */
    bool is_valid() const;

    /**
     * @brief Computes the air density at a given altitude, used in the drag equation
     * @param altitude_m Altitude above the launch pad, not sea level
     * @param density_out Filled with computed air density
     * @return true if successful, false otherwise
     */
    bool density_kgm3(double altitude_m, double& density_out) const;

    /**
     * @brief Sim barometer for SITL testing, computes the pressure and temperature at a given altitude
     * @param altitude_m Altitude above the launch pad, not sea level
     * @param conditions_out Filled with the computed pressure and temperature at altitude_m
     * @return true if successful, false otherwise
     */
    bool sim_barometer(double altitude_m,
                       AtmosphereConditions& conditions_out) const;

private:
    AtmosphereConditions
        launch_conditions;  // Measured pressure and temperature at launch site
};
}  // namespace LBR