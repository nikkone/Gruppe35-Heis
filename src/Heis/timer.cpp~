#include "timer.hpp"
timer::timer() {
    endTime = 0;
}
void timer::set(int seconds) {
    endTime = time(0)+seconds;
}
int timer::check(){
    if(endTime==0) {
        return 0;
    } else if(time(0) >= endTime) {
        endTime=0;
        return 1;
    }
    return 0;
}
