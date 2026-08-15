#ifndef ROCKET_DYNAMICS_H
#define ROCKET_DYNAMICS_H

typedef struct {
    double altitude;
    double velocity;
    double acceleration;
} RocketState;

void update_rocket (
    RocketState *state,
    double dt,
    double mass,
    double thrust,
    double air_density,
    double rocket_cd,
    double rocket_area,
    double flap_cd,
    double flap_area,
    double flap_angle_deg
);

#endif