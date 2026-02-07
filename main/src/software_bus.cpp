#include "software_bus.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/* Software bus queues */
QueueHandle_t imu_bus   = nullptr;
QueueHandle_t motor_bus = nullptr;

void software_bus_init()
{
    imu_bus   = xQueueCreate(1, sizeof(ImuData));
    motor_bus = xQueueCreate(1, sizeof(MotorCmd));
}
