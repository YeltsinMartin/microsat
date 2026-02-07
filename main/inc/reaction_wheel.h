#pragma once

#include <stdint.h>

class ReactionWheel {
public:
    void init();
    void step(float dt);

    /* commanded by IO_MGR */
    float command = 0.0f;

private:
    uint32_t toPwm(float speed);
};
