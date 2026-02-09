#include "aocs_mgr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <math.h>

extern QueueHandle_t imu_bus;
extern QueueHandle_t motor_bus;

void AOCSMgr::init()
{
    wheel_speed = 0.0f;
    gyro_bias   = 0.0f;
}

/* ---------------------------------------------------
 * Rate damping reaction wheel controller
 *
 * gyro rate → damping torque
 * torque → wheel acceleration
 * acceleration integrated → wheel speed
 *
 * wheel keeps momentum when torque = 0
 * --------------------------------------------------- */
void AOCSMgr::step(float dt)
{
    ImuData imu;

    if (!xQueuePeek(imu_bus, &imu, 0)) {
        return;
    }

    /* -----------------------------
     * 1. Gyro measurement
     * ----------------------------- */
    float gyro = imu.gy - gyro_bias;

    /* Remove small sensor noise */
    gyro = applyDeadband(gyro);

    /* -----------------------------
     * 2. Rate damping control
     * τ = -Kd * ω
     * ----------------------------- */
    float torque = -Kd * gyro;

    /* -----------------------------
     * 3. Torque → wheel acceleration
     * ----------------------------- */
    float accel = torque / J;

    /* -----------------------------
     * 4. Integrate to wheel speed
     * ----------------------------- */
    wheel_speed += accel * dt;

    wheel_speed = clamp(wheel_speed);

    /* -----------------------------
     * 5. Publish motor command
     * ----------------------------- */
    MotorCmd cmd;
    cmd.wheel_speed = wheel_speed;

    xQueueOverwrite(motor_bus, &cmd);
}

/* ---------------------------------------------------
 * Ignore tiny gyro noise
 * --------------------------------------------------- */
float AOCSMgr::applyDeadband(float rate)
{
    if (fabsf(rate) < RATE_DEADBAND)
        return 0.0f;

    return rate;
}

/* ---------------------------------------------------
 * Wheel speed safety limit
 * --------------------------------------------------- */
float AOCSMgr::clamp(float w)
{
    if (w > 5000.0f)  return 5000.0f;
    if (w < -5000.0f) return -5000.0f;
    return w;
}
