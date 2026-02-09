#pragma once

#include <stdint.h>

class ReactionWheel {

    uint32_t toPwm(float speed);
public:
    void init();
    void step(float dt);

    /* commanded by IO_MGR */
    float command = 0.0f;

};
