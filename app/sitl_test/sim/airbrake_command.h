#pragma once


// Current state of the airbrakes
struct AirbrakeState {
    double deployment = 0.0;
    // 0.0 = retracted
    // 1.0 = deployed
};