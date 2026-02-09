#pragma once

#include <stdbool.h>
#include "software_bus.h"

class HealthMgr {
public:
    void init();
    void step(float dt);

    bool faultActive();

private:
    u_int16_t  faultCounter ;
    float last_gyro = 0.0f;
    float timer = 0.0f;

    bool gyro_limit  = false;
    bool gyro_roc    = false;
    bool motor_limit = false;

    static constexpr float MAX_GYRO     = 60.0f;
    static constexpr float MAX_GYRO_ROC = 40.0f;
    static constexpr float MAX_MOTOR    = 5000.0f;

    void checkGyro(float dt);
    void checkMotor();
};
