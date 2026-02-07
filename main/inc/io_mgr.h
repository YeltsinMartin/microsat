#pragma once

#include "imu.h"
#include "reaction_wheel.h"
#include "software_bus.h"

class IOMgr {
public:
    void init();
    void step(float dt);

private:
    IMU imu;
    ReactionWheel reactionwheel;

    void publishImuData();
    void consumeMotorCmd();
};
