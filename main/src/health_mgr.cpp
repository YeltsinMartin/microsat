#include "health_mgr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <math.h>

extern QueueHandle_t imu_bus;
extern QueueHandle_t motor_bus;

void HealthMgr::init()
{
    last_gyro = 0.0f;
    timer = 0.0f;
    faultCounter = 0;
}

void HealthMgr::step(float dt)
{
    timer += dt;

    if (timer >= 0.2f) {   // 5 Hz gyro checks
        checkGyro(dt);
        timer = 0.0f;
    }

    checkMotor();
}

bool HealthMgr::faultActive()
{
    bool status = false;

    if (gyro_limit || gyro_roc || motor_limit ) 
    {
        faultCounter++;
        printf("Fault counter: %d, gyro_limit: %d, gyro_roc: %d, motor_limit: %d \n", faultCounter, gyro_limit, gyro_roc, motor_limit);
        status = true;
    }

    return status;
}

void HealthMgr::checkGyro(float dt)
{
    ImuData imu;

    if (xQueuePeek(imu_bus, &imu, 0)) {

        gyro_limit = fabsf(imu.gy) > MAX_GYRO;

        float roc = fabsf(imu.gy - last_gyro) / dt;
        gyro_roc = roc > MAX_GYRO_ROC;

        last_gyro = imu.gy;
    }
}

void HealthMgr::checkMotor()
{
    MotorCmd cmd;

    if (xQueuePeek(motor_bus, &cmd, 0)) {
        motor_limit = fabsf(cmd.wheel_speed) > MAX_MOTOR;
    }
}

