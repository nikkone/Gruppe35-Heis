
#include <stdlib.h>

#include "fsm.h"
#include "orderHandling.h"
#include "timer.h"

static state_t elevatorState = IDLE;
static elev_motor_direction_t direction = DIRN_STOP;
static int currentFloor = -1;
static int nextFloor = -1;

static void fsm_setState(state_t nextState);
static void fsm_resetFloorLights(int floor);
static void fsm_updateDirection(void);

void fsm_setState(state_t nextState) {
    elevatorState = nextState;
    switch (nextState) {
        case STOP:
            elev_set_stop_lamp(ON);
            elev_set_motor_direction(DIRN_STOP);
            oh_resetAllOrders();
            for (int floor = 0; floor < N_FLOORS; floor++) {
                fsm_resetFloorLights(floor);
            }
            break;
        case IDLE:
            elev_set_motor_direction(DIRN_STOP);
            elev_set_stop_lamp(OFF);
            elev_set_door_open_lamp(OFF);
            break;
        case RUNNING:
            elev_set_motor_direction(direction);
            break;
        case DOOR_OPEN:
            elev_set_door_open_lamp(ON);
            elev_set_motor_direction(DIRN_STOP);
            timer_start(3);
            break;
        case UNDEFINED:
            elev_set_motor_direction(DIRN_UP);
            elev_set_door_open_lamp(OFF);
            elev_set_stop_lamp(OFF);
            nextFloor = -1;
            break; 
        case UNINITIALIZED:
            if (!elev_init()) {
                exit(1);
            }
            elev_set_motor_direction(DIRN_UP);
            while(elev_get_floor_sensor_signal() == -1);
            elev_set_motor_direction(DIRN_STOP);
            oh_resetAllOrders();
            break; 
    }
}

void fsm_resetFloorLights(int floor) {
    elev_set_button_lamp(BUTTON_COMMAND, floor, OFF);
    switch (floor) {
        case 0:
            elev_set_button_lamp(BUTTON_CALL_UP, floor, OFF);
            break;
        case N_FLOORS-1:
            elev_set_button_lamp(BUTTON_CALL_DOWN, floor, OFF);
            break;
        default:
            elev_set_button_lamp(BUTTON_CALL_UP, floor, OFF);
            elev_set_button_lamp(BUTTON_CALL_DOWN, floor, OFF);
            break;
    }
}

void fsm_updateDirection(void) {
    if (elevatorState == IDLE) {
        nextFloor = oh_getNextOrder(currentFloor, direction);
        if (nextFloor == -1) {
            direction = DIRN_STOP;
        } else {
            direction = (nextFloor - currentFloor);
            if (direction < 0) {
                direction = DIRN_DOWN;
            } else if (direction > 0){
                direction = DIRN_UP;
            }

            if (direction != DIRN_STOP) {
                fsm_setState(RUNNING);
            }
        }
    }
}

void fsm_uninitializedHardware(void) {
    fsm_setState(UNINITIALIZED);
    fsm_setState(IDLE);
}

void fsm_buttonPressed(int floor, elev_button_type_t buttonType) {
    if (elevatorState == STOP) {
        fsm_setState(UNDEFINED);
    } else {
        oh_setOrder(floor, buttonType);
        elev_set_button_lamp(buttonType, floor, ON);
        nextFloor = oh_getNextOrder(currentFloor, direction);
    }
}

void fsm_sensorActivated(int floor) {
    if (elevatorState == UNDEFINED) {
        fsm_setState(IDLE);
    }
    elev_set_floor_indicator(floor);
    if ( oh_isFloorOrdered(floor, direction, nextFloor) ) {
        oh_resetFloorOrders(floor);
        fsm_resetFloorLights(floor);
        if (elevatorState == RUNNING || elevatorState == IDLE) {
            nextFloor = oh_getNextOrder(currentFloor, direction);
            fsm_setState(DOOR_OPEN);
        }
    } else if (elevatorState == STOP) {
        elev_set_door_open_lamp(ON);
    }
    currentFloor = floor;
    fsm_updateDirection();
}

void fsm_stopButtonPressed(void) {
    fsm_setState(STOP);
}

void fsm_stopButtonReleased(void) {
    if(elevatorState == STOP) {
        elev_set_stop_lamp(OFF);
        elev_set_door_open_lamp(OFF);
    }
}

void fsm_timerTimedOut(void) {
    if(elevatorState == DOOR_OPEN) {
        fsm_setState(IDLE);    
    }
}
