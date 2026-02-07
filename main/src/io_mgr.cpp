#include "io_mgr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t imu_bus;
extern QueueHandle_t motor_bus;

void IOMgr::init()
{
    imu.init();
    reactionwheel.init();
}

void IOMgr::step(float dt)
{
    imu.step(dt);
    publishImuData();

    consumeMotorCmd();
    reactionwheel.step(dt);
}

void IOMgr::publishImuData()
{
    xQueueOverwrite(imu_bus, &imu.data);
}

void IOMgr::consumeMotorCmd()
{
    MotorCmd cmd;
    if (xQueuePeek(motor_bus, &cmd, 0)) {
        reactionwheel.command = cmd.wheel_speed;
    }
}
