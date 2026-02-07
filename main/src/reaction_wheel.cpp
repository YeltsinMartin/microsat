#include "reaction_wheel.h"
#include "driver/pwm.h"

#define PWM_0_OUT_IO_NUM   1

void ReactionWheel::init()
{
    // pwm pin number
    const uint32_t pin_num[1] = {
        PWM_0_OUT_IO_NUM
    };
    uint32_t duty = 1000;

    pwm_init(50, &duty, 1, pin_num);   // 50 Hz ESC
}

void ReactionWheel::step(float /*dt*/)
{
    uint32_t duty = toPwm(command);
    pwm_set_duty(0, duty);
    pwm_start();
}

uint32_t ReactionWheel::toPwm(float speed)
{
    uint32_t pwm = static_cast<uint32_t>(1500.0f + speed * 0.1f);

    if (pwm < 1000) pwm = 1000;
    if (pwm > 2000) pwm = 2000;

    return pwm;
}
