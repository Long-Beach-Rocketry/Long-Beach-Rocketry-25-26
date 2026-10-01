# Rocket Dynamics Model: 2D point-mass flight physics. Pure math, no I/O and
# no hardware dependency, so the library itself is unconditional -- it can
# be linked by hardware targets too (e.g. an onboard apogee predictor using
# the same physics), not just NATIVE/SITL simulation. Only the unit test
# below is NATIVE-only, matching common/core/container's pattern.

add_library(rocket_dynamics STATIC
    rocket_dynamics.cc
)

target_include_directories(rocket_dynamics PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

if("${TARGET_DEVICE}" MATCHES "NATIVE")
    add_executable(rocket_dynamics_test
        rocket_dynamics_test.cc
    )

    target_link_libraries(rocket_dynamics_test
        PRIVATE
            rocket_dynamics
            GTest::gtest_main
            pthread
    )

    add_test(NAME rocket_dynamics_test COMMAND rocket_dynamics_test)
endif()