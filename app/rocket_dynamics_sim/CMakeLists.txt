# SITL test harness for the Rocket Dynamics Model + Atmosphere Model
# libraries (common/core/rocket_dynamics, common/core/atmosphere). This is
# an executable, not a library -- it stands in for every other pipeline box
# that doesn't exist yet (Airbrake Logic, Actuator Model, MPC, whatever
# supplies Dynamic Sim Properties) so those two real libraries can be
# exercised end to end. NATIVE/SITL-only: this harness has no reason to run
# on hardware, unlike the libraries it links against.
if("${TARGET_DEVICE}" MATCHES "^(NATIVE|SITL)$")

    add_executable(rocket_dynamics_sim
        main.cc
    )

    target_link_libraries(rocket_dynamics_sim PRIVATE
        rocket_dynamics
        atmosphere
    )

endif()