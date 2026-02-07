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
}

void HealthMgr::step(float dt)
{
    timer += dt;

    if (timer >= 0.2f) {   // 5 Hz gyro checks
        checkGyro();
        timer = 0.0f;
    }

    checkMotor();
}

bool HealthMgr::faultActive() const
{
    return gyro_limit || gyro_roc || motor_limit;
}

void HealthMgr::checkGyro()
{
    ImuData imu;

    if (xQueuePeek(imu_bus, &imu, 0)) {

        gyro_limit = fabsf(imu.gz) > MAX_GYRO;

        float roc = fabsf(imu.gz - last_gyro) / 0.2f;
        gyro_roc = roc > MAX_GYRO_ROC;

        last_gyro = imu.gz;
    }
}

void HealthMgr::checkMotor()
{
    MotorCmd cmd;

    if (xQueuePeek(motor_bus, &cmd, 0)) {
        motor_limit = fabsf(cmd.wheel_speed) > MAX_MOTOR;
    }
}
