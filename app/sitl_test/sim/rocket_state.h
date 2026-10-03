#pragma once
#include <cstdint>

// Represents the state of the rocket (prelaunch, launched, airbrakes deployed, etc.)
enum class RocketState : uint8_t
{
    PRELAUNCH,
    LAUNCHED,
    BRAKES_DEPLOYING,
    BRAKES_DEPLOYED,
    BRAKES_RETRACTING,
    BRAKES_RETRACTED,
    RECOVERY_POPPED,
    LANDED
};