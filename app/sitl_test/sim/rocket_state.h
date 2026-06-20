#pragma once

// Represents the state of the rocket (prelaunch, launched, airbrakes deployed, etc.)
enum class RocketState
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