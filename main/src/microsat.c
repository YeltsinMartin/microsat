#include "tasks.h"
#include "software_bus.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("Hello world!\n");
    
    software_bus_init();

    xTaskCreate(io_task,     "IO_MGR",     2048, NULL, 3, NULL);
    xTaskCreate(aocs_task,   "AOCS_MGR",   2048, NULL, 2, NULL);
    xTaskCreate(health_task, "HEALTH_MGR", 2048, NULL, 1, NULL);
}
