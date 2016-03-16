#include <unistd.h>

#include "fsm.h"
#include "elev.h"
#include "timer.h"

int main(void) {
    fsm_uninitializedHardware();
    int sensorSignal;
    while(1) {
        sensorSignal = elev_get_floor_sensor_signal();
        if (sensorSignal >= 0) {
            fsm_sensorActivated(sensorSignal);
        }

        if(elev_get_stop_signal()){
            fsm_stopButtonPressed();
        } else {
            fsm_stopButtonReleased();

            for (int floor = 0; floor < N_FLOORS; floor++) {
                if (elev_get_button_signal(BUTTON_COMMAND, floor)) {
                    fsm_buttonPressed(floor, BUTTON_COMMAND);                
                }
            }
            
            for (int floor = 1; floor < N_FLOORS; floor++) {
                if (elev_get_button_signal(BUTTON_CALL_DOWN, floor)) {
                    fsm_buttonPressed(floor, BUTTON_CALL_DOWN);
                }
            }

            for (int floor = 0; floor < N_FLOORS-1; floor++) {
                if (elev_get_button_signal(BUTTON_CALL_UP, floor)) {
                    fsm_buttonPressed(floor, BUTTON_CALL_UP);
                }
            }
        }

        if (timer_finished()) {
            fsm_timerTimedOut();
        }

        usleep(1000);
    }

    return 0;
}
