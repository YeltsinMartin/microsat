#pragma once

#include "software_bus.h"

class AOCSMgr {
public:
    void init();
    void step(float dt);

private:
    /* State */
    float theta_est   = 0.0f;
    float theta_ref   = 0.0f;
    float wheel_speed = 0.0f;

    /* Gyro bias (set at startup if available) */
    float gyro_bias = 0.0f;
    /* Controller gains */
    float Kp = 1.55f;   // was 0.35
    float Kd = 3.12f;   // was 0.08
    float WHEEL_DAMP = 0.18f; // was 0.25
    float J = 0.008f;   // was 0.01

    /* Deadband (rad) */
    float RATE_DEADBAND = 2.0f;

    float applyDeadband(float err);
    float clamp(float w);
};
