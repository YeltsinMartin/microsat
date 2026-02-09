#pragma once
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ImuData {
    float ax, ay, az, gx, gy, gz;
};

struct MotorCmd {
    float wheel_speed; 
};

extern QueueHandle_t imu_bus;
extern QueueHandle_t motor_bus;

void software_bus_init(void);

#ifdef __cplusplus
}
#endif
