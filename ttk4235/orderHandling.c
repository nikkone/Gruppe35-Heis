#include <stdlib.h>

#include "orderHandling.h"

static int orders[N_FLOORS][N_BUTTONS];

void oh_setOrder(int floor, elev_button_type_t buttonType){
    orders[floor][buttonType] = 1;
}

void oh_resetFloorOrders(int floor) {
    for (int button = 0; button < N_BUTTONS; button++) {
        orders[floor][button] = 0;
    }
}

void oh_resetAllOrders(void) {
    for (int floor = 0; floor < N_FLOORS; floor++) {
        for (int button = 0; button < N_BUTTONS; button++) {
            orders[floor][button] = 0;
        }
    }
}

int oh_getNextOrder(int currentFloor, elev_motor_direction_t direction) {
    int lowerFloor, upperFloor;    
    switch (direction) {
        case DIRN_DOWN:
            lowerFloor = 0;
            upperFloor = currentFloor;
            break;
        case DIRN_UP:
            lowerFloor = currentFloor;
            upperFloor = N_FLOORS-1;
            break;
        case DIRN_STOP:
            lowerFloor = 0;
            upperFloor = N_FLOORS-1;
            break;
    }
    int nextFloor = -1;    
    for (int floor = upperFloor; floor >= lowerFloor; floor--) {
        for (int button = 0; button < N_BUTTONS; button++) {        
            if (orders[floor][button]) {
                if (nextFloor == -1) {
                    nextFloor = floor;            
                }
                else if (abs(currentFloor - floor) > abs(currentFloor - nextFloor)) {
                    nextFloor = floor;            
                }
            }
        }                     
    }
    return nextFloor;
}

int oh_isFloorOrdered(int floor, elev_motor_direction_t direction, int nextFloor){
    if(orders[floor][BUTTON_COMMAND]) {
        return 1;
    }
    if (floor == nextFloor) {
        return 1;
    }
    switch (direction) {
        case DIRN_DOWN:
            if(orders[floor][BUTTON_CALL_DOWN]) {
                return 1;
            }
            break;
        case DIRN_UP:
            if(orders[floor][BUTTON_CALL_UP]) {
                return 1;
            }
            break;
        case DIRN_STOP:
            if(orders[floor][BUTTON_CALL_DOWN]) {
                return 1;
            }
            if(orders[floor][BUTTON_CALL_UP]) {
                return 1;
            }
            break;
    }
    if(floor == 0) {
        if(orders[floor][BUTTON_CALL_UP]) {
            return 1;
        }
    }
    if(floor == N_FLOORS-1) {
        if(orders[floor][BUTTON_CALL_DOWN]) {
            return 1;
        }
    }
    return 0;
}