# Atmosphere: shared ISA physics (atmosphere.cc) + the Atmosphere Model
# pipeline box (atmosphere_model.cc, true altitude -> true barometer raw
# values). Pure math, no I/O, no hardware dependency -- unconditional
# library, same reasoning as rocket_dynamics/CMakeLists.txt. Only the unit
# test below is NATIVE-only.

add_library(atmosphere STATIC
    atmosphere.cc
    atmosphere_model.cc
)

target_include_directories(atmosphere PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

if("${TARGET_DEVICE}" MATCHES "NATIVE")
    add_executable(atmosphere_test
        atmosphere_test.cc
    )

    target_link_libraries(atmosphere_test
        PRIVATE
            atmosphere
            GTest::gtest_main
            pthread
    )

    add_test(NAME atmosphere_test COMMAND atmosphere_test)
endif()