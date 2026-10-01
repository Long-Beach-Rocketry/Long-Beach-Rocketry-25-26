/**
 * @file rocket_dynamics_test.cc
 * @brief AI-generated unit tests for rocket_dynamics.h/.cc, verifying core
 *        physics (free fall, thrust, and airbrake drag) against expected values.
 */

#include "rocket_dynamics.h"
#include <gtest/gtest.h>

namespace {

RocketVariables make_state_at_rest()
{
    RocketVariables state = {};
    state.time_s = 0.0;
    state.x_m = 0.0;
    state.y_m = 0.0;
    state.velocity_x_mps = 0.0;
    state.velocity_y_mps = 0.0;
    state.acceleration_x_mps2 = 0.0;
    state.acceleration_y_mps2 = 0.0;
    state.mass_kg = 20.0;
    return state;
}

RocketStaticProperties make_zero_drag_properties()
{
    // Zero areas/coefficients -> no drag contribution at all, isolating
    // gravity and thrust for these tests.
    RocketStaticProperties property = {};
    property.rocket_coefficient_of_drag = 0.0;
    property.rocket_reference_area_m2 = 0.0;
    property.flap_coefficient_of_drag = 0.0;
    property.flap_reference_area_m2 = 0.0;
    property.number_of_flaps = 0;
    property.flap_angle_deg = 0.0;
    return property;
}

}  // namespace

/**
 * @brief With zero thrust and zero drag, the rocket should be in free fall:
 * vertical velocity should decrease by exactly g * dt after one step, and
 * horizontal velocity/position should be unaffected.
 */
TEST(RocketDynamicsTest, FreeFallMatchesGravity)
{
    const RocketVariables current = make_state_at_rest();
    const RocketStaticProperties property = make_zero_drag_properties();

    RocketDynamicProperties sim = {};
    sim.dt_s = 0.1;
    sim.gravity_mps2 = 9.81;
    sim.thrust_n = 0.0;
    sim.body_angle_deg = 0.0;
    sim.air_density_kgm3 = 1.225;

    const RocketVariables next = update_rocket_dynamics(&current, &property, &sim);

    EXPECT_NEAR(next.acceleration_y_mps2, -sim.gravity_mps2, 1e-9)
        << "Vertical acceleration should equal -g with no thrust or drag.";
    EXPECT_NEAR(next.velocity_y_mps, -sim.gravity_mps2 * sim.dt_s, 1e-9)
        << "Vertical velocity should decrease by exactly g * dt.";
    EXPECT_NEAR(next.velocity_x_mps, 0.0, 1e-9)
        << "Horizontal velocity should be unaffected with zero thrust/drag.";
    EXPECT_NEAR(next.x_m, 0.0, 1e-9);
}

/**
 * @brief With thrust straight up (body_angle_deg = 0) and zero drag,
 * vertical acceleration should be exactly thrust/mass - g.
 */
TEST(RocketDynamicsTest, ThrustNoDragMatchesNewtonsSecondLaw)
{
    const RocketVariables current = make_state_at_rest();
    const RocketStaticProperties property = make_zero_drag_properties();

    RocketDynamicProperties sim = {};
    sim.dt_s = 0.1;
    sim.gravity_mps2 = 9.81;
    sim.thrust_n = 800.0;
    sim.body_angle_deg = 0.0;
    sim.air_density_kgm3 = 1.225;

    const RocketVariables next = update_rocket_dynamics(&current, &property, &sim);

    const double expected_accel_y = sim.thrust_n / current.mass_kg - sim.gravity_mps2;
    EXPECT_NEAR(next.acceleration_y_mps2, expected_accel_y, 1e-9)
        << "Vertical acceleration should be thrust/mass - g with zero drag.";
    EXPECT_GT(next.velocity_y_mps, 0.0)
        << "With 800N thrust on a 20kg rocket, it should accelerate upward.";
}

/**
 * @brief Deploying the airbrake flaps (nonzero flap angle) at the same
 * speed should increase drag area and therefore reduce upward acceleration
 * compared to flaps retracted, all else equal.
 */
TEST(RocketDynamicsTest, DeployedFlapsIncreaseDragVsRetracted)
{
    RocketVariables current = make_state_at_rest();
    current.velocity_y_mps = 100.0;  // give drag something to act on

    RocketStaticProperties retracted = {};
    retracted.rocket_coefficient_of_drag = 0.5;
    retracted.rocket_reference_area_m2 = 0.01;
    retracted.flap_coefficient_of_drag = 1.0;
    retracted.flap_reference_area_m2 = 0.0005;
    retracted.number_of_flaps = 4;
    retracted.flap_angle_deg = 0.0;

    RocketStaticProperties deployed = retracted;
    deployed.flap_angle_deg = 30.0;

    RocketDynamicProperties sim = {};
    sim.dt_s = 0.1;
    sim.gravity_mps2 = 9.81;
    sim.thrust_n = 0.0;
    sim.body_angle_deg = 0.0;
    sim.air_density_kgm3 = 1.225;

    const RocketVariables next_retracted = update_rocket_dynamics(&current, &retracted, &sim);
    const RocketVariables next_deployed = update_rocket_dynamics(&current, &deployed, &sim);

    EXPECT_LT(next_deployed.acceleration_y_mps2, next_retracted.acceleration_y_mps2)
        << "Deployed flaps should produce more negative (more decelerating) "
           "vertical acceleration than retracted flaps at the same speed.";
}