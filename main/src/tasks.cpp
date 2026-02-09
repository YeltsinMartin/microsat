#include "tasks.h"
#include "io_mgr.h"
#include "aocs_mgr.h"
#include "health_mgr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Static app instances */
static IOMgr     io_mgr;
static AOCSMgr   aocs_mgr;
static HealthMgr health_mgr;

void io_task(void *arg)
{
    io_mgr.init();
    TickType_t last = xTaskGetTickCount();

    while (1) {
        //printf("io_task\n");
        io_mgr.step(0.01f);
        vTaskDelayUntil(&last, pdMS_TO_TICKS(10));
    }
}

void aocs_task(void *arg)
{
    aocs_mgr.init();
    TickType_t last = xTaskGetTickCount();

    while (1) {
        //printf("aocs_task\n");
        aocs_mgr.step(0.01f);
        vTaskDelayUntil(&last, pdMS_TO_TICKS(10));
    }
}

void health_task(void *arg)
{
    health_mgr.init();
    TickType_t last = xTaskGetTickCount();

    while (1) {
        //printf("health_task\n");
        health_mgr.step(0.2f);

        if (health_mgr.faultActive()) {
            /* No recovery here by design */
        }

        vTaskDelayUntil(&last, pdMS_TO_TICKS(200));
    }
}
