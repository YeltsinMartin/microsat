#include "aocs_mgr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <math.h>

extern QueueHandle_t imu_bus;
extern QueueHandle_t motor_bus;

void AOCSMgr::init()
{
    theta_est   = 0.0f;
    theta_ref   = 0.0f;
    wheel_speed = 0.0f;
}

void AOCSMgr::step(float dt)
{
    ImuData imu;

    if (!xQueuePeek(imu_bus, &imu, 0)) {
        return;
    }

    /* ------------------------------------
     * 1. Attitude estimation
     * ------------------------------------ */
    theta_est += imu.gz * dt;

    /* ------------------------------------
     * 2. Error with deadband
     * ------------------------------------ */
    float theta_err = theta_est - theta_ref;
    float theta_eff = applyDeadband(theta_err);

    /* ------------------------------------
     * 3. PD control law
     * ------------------------------------ */
    float torque =
        -Kp * theta_eff
        -Kd * imu.gz;

    /* ------------------------------------
     * 4. Torque → wheel speed
     * ------------------------------------ */
    wheel_speed += (torque / J) * dt;
    wheel_speed = clamp(wheel_speed);

    /* ------------------------------------
     * 5. Publish command
     * ------------------------------------ */
    MotorCmd cmd;
    cmd.wheel_speed = wheel_speed;
    xQueueOverwrite(motor_bus, &cmd);
}

float AOCSMgr::applyDeadband(float err)
{
    if (fabsf(err) < THETA_DEADBAND) {
        return 0.0f;
    }

    /* Soft deadband (no discontinuity) */
    return err - copysignf(THETA_DEADBAND, err);
}

float AOCSMgr::clamp(float w)
{
    if (w > 5000.0f)  return 5000.0f;
    if (w < -5000.0f) return -5000.0f;
    return w;
}
