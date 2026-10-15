#pragma once

// Represents the state of the airbrakes (retracted or deployed)
struct AirbrakeState
{
    bool deployment = false;
    // false = retracted
    // true = deployed
};