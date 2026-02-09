#include "reaction_wheel.h"

extern "C" {
#include "driver/pwm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
}

#define PWM_ESC_OUT_IO_NUM   12
#define PWM_PERIOD_US   20000
#define ESC_MIN_US      1000
#define ESC_MAX_US      2000

static uint32_t duties[1] = {ESC_MIN_US};
static uint32_t pins[1]   = {PWM_ESC_OUT_IO_NUM};

void ReactionWheel::init()
{

    ESP_ERROR_CHECK(pwm_init(PWM_PERIOD_US, duties, 1, pins));

    pwm_start();

    vTaskDelay(pdMS_TO_TICKS(3000));

    pwm_set_duty(0, ESC_MIN_US);
    pwm_start();

    vTaskDelay(pdMS_TO_TICKS(1000));
}

void ReactionWheel::step(float /*dt*/)
{
    uint32_t duty = toPwm(command);

    //printf("PWM : %d  Command :%d \n", duty, (int32_t)command);
    pwm_set_duty(0, duty);
    pwm_start();
}

uint32_t ReactionWheel::toPwm(float speed)
{
    if (speed > 5000) speed = 5000;
    if (speed < -5000) speed = -5000;

    float pwm = 1200.0f +
                (speed + 5000.0f) * 750.0f / 10000.0f;

    return (uint32_t)pwm;
}