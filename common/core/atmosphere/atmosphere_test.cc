/**
 * @file atmosphere_test.cc
 * @brief AI-generated unit tests for atmosphere.h/.cc and
 *        atmosphere_model.h/.cc, checking the ISA math and the Atmosphere
 *        Model wrapper against known/derivable values.
 */

#include "atmosphere.h"
#include "atmosphere_model.h"
#include <gtest/gtest.h>

namespace {

GroundConditions make_sea_level_ground()
{
    GroundConditions ground;
    ground.temperature_k = ISA_SEA_LEVEL_TEMPERATURE_K;
    ground.pressure_pa = ISA_SEA_LEVEL_PRESSURE_PA;
    return ground;
}

}  // namespace

/**
 * @brief At height 0 above the ground, the model should return exactly the
 * given ground conditions -- no lapse-rate adjustment should be applied.
 */
TEST(AtmosphereTest, ZeroHeightMatchesGroundConditionsExactly)
{
    const GroundConditions ground = make_sea_level_ground();

    EXPECT_DOUBLE_EQ(atmosphere_temperature_k(0.0, &ground), ground.temperature_k);
    EXPECT_DOUBLE_EQ(atmosphere_pressure_pa(0.0, &ground), ground.pressure_pa);
}

/**
 * @brief ground_conditions_for_elevation_m(0) should reproduce the
 * ISA sea-level reference values exactly, since 0m elevation above sea
 * level is, by definition, sea level.
 */
TEST(AtmosphereTest, StandardGroundConditionsAtZeroElevationIsSeaLevel)
{
    const GroundConditions ground = ground_conditions_for_elevation_m(0.0);

    EXPECT_DOUBLE_EQ(ground.temperature_k, ISA_SEA_LEVEL_TEMPERATURE_K);
    EXPECT_DOUBLE_EQ(ground.pressure_pa, ISA_SEA_LEVEL_PRESSURE_PA);
}

/**
 * @brief Temperature and pressure should both decrease monotonically with
 * height, matching the physical expectation for the troposphere.
 */
TEST(AtmosphereTest, TemperatureAndPressureDecreaseWithHeight)
{
    const GroundConditions ground = make_sea_level_ground();

    const double temp_low = atmosphere_temperature_k(100.0, &ground);
    const double temp_high = atmosphere_temperature_k(2000.0, &ground);
    EXPECT_GT(temp_low, temp_high) << "Temperature should drop with altitude.";

    const double pressure_low = atmosphere_pressure_pa(100.0, &ground);
    const double pressure_high = atmosphere_pressure_pa(2000.0, &ground);
    EXPECT_GT(pressure_low, pressure_high) << "Pressure should drop with altitude.";
}

/**
 * @brief Density should be derivable from pressure and temperature via the
 * ideal gas law -- cross-check atmosphere_density_kgm3 against a manual
 * computation from atmosphere_pressure_pa/atmosphere_temperature_k.
 */
TEST(AtmosphereTest, DensityMatchesIdealGasLaw)
{
    const GroundConditions ground = make_sea_level_ground();
    constexpr double kSpecificGasConstantDryAir = 287.05;
    constexpr double kHeight = 1500.0;

    const double density = atmosphere_density_kgm3(kHeight, &ground);
    const double pressure = atmosphere_pressure_pa(kHeight, &ground);
    const double temperature = atmosphere_temperature_k(kHeight, &ground);
    const double expected_density = pressure / (kSpecificGasConstantDryAir * temperature);

    EXPECT_NEAR(density, expected_density, 1e-9);
}

/**
 * @brief The Atmosphere Model box's output should exactly match the
 * underlying atmosphere.c physics functions -- it's meant to be a thin,
 * noise-free wrapper, nothing more.
 */
TEST(AtmosphereModelTest, BarometerRawValuesMatchUnderlyingPhysics)
{
    const GroundConditions ground = ground_conditions_for_elevation_m(1200.0);
    constexpr double kAltitude = 500.0;

    const BarometerRawValues values = compute_barometer_raw_values(kAltitude, &ground);

    EXPECT_DOUBLE_EQ(values.pressure_pa, atmosphere_pressure_pa(kAltitude, &ground));
    EXPECT_DOUBLE_EQ(values.temperature_k, atmosphere_temperature_k(kAltitude, &ground));
}