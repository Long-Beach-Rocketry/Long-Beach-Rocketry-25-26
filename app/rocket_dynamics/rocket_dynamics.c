#include "rocket_dynamics.h"
#include <math.h>

#define GRAVITY 9.81
#define DEG_TO_RAD (M_PI / 180.0)

void update_rocket (
    RocketState *rocket,
    double dt,
    double mass,
    double thrust,
    double air_density,
    double rocket_cd,
    double rocket_area,
    double flap_cd,
    double flap_area,
    double flap_angle_deg
)

{
    double flap_angle_rad = flap_angle_deg * DEG_TO_RAD;

    double effective_flap_area = flap_area * sin(flap_angle_rad);

    double total_cd_area = (rocket_cd * rocket_area) + (2.0 * flap_cd * effective_flap_area);

    double drag = 0.5 * air_density * total_cd_area * rocket->velocity * rocket->velocity;

    double force = thrust - drag - (mass * GRAVITY);

    rocket->acceleration = force / mass;

    rocket->velocity += rocket->acceleration * dt;

    rocket->altitude += rocket->velocity * dt;
}