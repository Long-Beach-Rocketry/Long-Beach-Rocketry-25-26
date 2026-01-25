#include "pps_helpers.h"

void motorDeploy() {
    
}

void motorTarget() {
    // TODO: Implement motor target direction (stub)
    // Called during Rotate state to move mechanism to dig position.
    // Should use PWM to rotate motor to target angle/position.
}

void motorRetract() {
    // TODO: Implement motor retract direction (stub)
    // Called during Retract state to retract mechanism until retracted position is reached.
    // Should use PWM to drive motor backward.
} 
// Use the LBR::Motor interface for all helpers
#include "motor_support/dc_motor.h"

void motorDeploy(LBR::Motor& motor) {
    motor.motorEnable(true);
    motor.motorDirection(true); // true = deploy direction
    motor.motorSpeed(100);      // Full speed
}

void motorTarget(LBR::Motor& motor) {
    // Example: move to a target position (implement as needed)
    // motor.moveDegrees(target_degrees, speed);
}

void motorRetract(LBR::Motor& motor) {
    motor.motorEnable(true);
    motor.motorDirection(false); // false = retract direction
    motor.motorSpeed(100);       // Full speed reverse
}