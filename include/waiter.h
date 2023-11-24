#pragma once

#include "globals.h"

class Waiter {
private:
    int startTick = 0;
public:
    bool wait(int ticks) {
        startTick = (startTick == 0) ? gameTicks : startTick;
        if (gameTicks >= startTick + ticks) {
            startTick = 0;
            return true;
        }
        return false;
    }
};