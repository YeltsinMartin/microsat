#pragma once
#include "software_bus.h"
#include "MPU6050.h"
class IMUData;

class IMU {

    alignas(MPU6050) uint8_t mpu_buffer[sizeof(MPU6050)];
    MPU6050* mpu = NULL;

public:
    void init();
    void step(float dt);

    /* owned state – read by IO_MGR only */
    float angular_rate = 0.0f;

    ImuData data;

};
