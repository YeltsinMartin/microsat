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

    /* Controller gains */
    static constexpr float Kp = 0.8f;
    static constexpr float Kd = 0.15f;

    /* Reaction wheel inertia */
    static constexpr float J  = 0.002f;

    /* Deadband (rad) */
    static constexpr float THETA_DEADBAND = 0.0087f; // ~0.5 deg

    float applyDeadband(float err);
    float clamp(float w);
};
