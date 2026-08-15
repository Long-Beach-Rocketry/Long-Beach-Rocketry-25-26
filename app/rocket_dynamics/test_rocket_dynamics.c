#include "rocket_dynamics.h"
#include <stdio.h>

int main(void) {
    RocketState rocket = {.altitude = 0.0, .velocity = 100.0, .acceleration = 0.0};

    double dt = 0.1;
    double time = 0.0;
    
    double mass = 30.0;
    double air_density = 1.225;
    double rocket_cd = 0.5;
    double rocket_area = 0.02;
    double flap_cd = 0.5;
    double flap_area = 0.01;

    double thrust = 0.0;
    double flap_angle_deg = 30.0;

    printf("Starting rocket simulation... \n");
    printf("Time(s)   Altitude(m)   Velocity(m/s)   Acceleration(m/s^2)   Flap(deg)\n");
    printf("--------------------------------------------------------------------------\n");
    while (rocket.velocity > 0.0) {
        if (time < 6.0)
        {
            flap_angle_deg = 0.0;
        }
        else
        {
            flap_angle_deg = 30.0;
        }

        update_rocket(&rocket, dt, mass, thrust, air_density, rocket_cd, rocket_area, flap_cd, flap_area, flap_angle_deg);
        time += dt;

        printf("%7.2f   %11.2f   %13.2f   %19.2f   %9.1f\n", time, rocket.altitude, rocket.velocity, rocket.acceleration, flap_angle_deg);
    }
    printf("APOGEE REACHED! \n");
    printf("Final Time: %.2f s\n", time);
    printf("Final Altitude: %.2f m\n", rocket.altitude);
    printf("Final Velocity: %.2f m/s\n", rocket.velocity);
    printf("Final Acceleration: %.2f m/s^2\n", rocket.acceleration);

    return 0;
}
