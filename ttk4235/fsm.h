#ifndef __INCLUDE_FSM_H__
#define __INCLUDE_FSM_H__

#include "elev.h"

typedef enum state {
    STOP, IDLE, RUNNING, DOOR_OPEN, UNDEFINED, UNINITIALIZED
} state_t;

enum toggle {
	OFF, ON
};

void fsm_uninitializedHardware(void);
void fsm_buttonPressed(int floor, elev_button_type_t buttonType);
void fsm_sensorActivated(int floor);
void fsm_stopButtonPressed(void);
void fsm_stopButtonReleased(void);
void fsm_timerTimedOut(void);

#endif //#ifndef __INCLUDE_FSM_H__
