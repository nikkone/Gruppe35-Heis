#include <time.h>

#include "timer.h"

static time_t endTime = 0;

void timer_start(int seconds) {
    endTime = time(0) + seconds;
}

int timer_finished(void){
    if(endTime == 0) {
        return 0;
    } else if (time(0) >= endTime) {
        endTime = 0;
        return 1;
    }
    return 0;
}
