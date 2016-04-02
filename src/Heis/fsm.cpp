#include "fsm.hpp"

//for cout
#include <iostream>

ElevatorFSM::ElevatorFSM(OrderList *elevator) {
	orders = elevator;
    destination = -1;
    setState(UNINITIALIZED);
}
void ElevatorFSM::setState(state_t nextState) {
    std::cout << (int)nextState << std::endl;
    elevatorState = nextState;
    switch (nextState) {
        case IDLE:
            elev_set_motor_direction(DIRN_STOP);
            elev_set_stop_lamp(OFF);
            elev_set_door_open_lamp(OFF);
            break;
        case RUNNING:
            //elev_set_motor_direction(direction);
            break;
        case DOOR_OPEN:
            elev_set_door_open_lamp(ON);
            elev_set_motor_direction(DIRN_STOP);
            //timer_start(3);
            break;
        case UNINITIALIZED:
            if (!elev_init()) {
                exit(1);
            }
            elev_set_motor_direction(DIRN_UP);
            while(elev_get_floor_sensor_signal() == -1);
            setState(IDLE);
            break; 
/*
        case STOP:
            elev_set_stop_lamp(ON);
            elev_set_motor_direction(DIRN_STOP);
            //oh_resetAllOrders();
            for (int floor = 0; floor < N_FLOORS; floor++) {
                resetFloorLights(floor);
            }
            break;
        case UNDEFINED:
            elev_set_motor_direction(DIRN_UP);
            elev_set_door_open_lamp(OFF);
            elev_set_stop_lamp(OFF);
            destination = -1;
            break; 
*/
    }
}
 void ElevatorFSM::resetFloorLights(int floor) {
    elev_set_button_lamp(BUTTON_COMMAND, floor, false);
    switch (floor) {
        case 0:
            elev_set_button_lamp(BUTTON_CALL_UP, floor, false);
            break;
        case N_FLOORS-1:
            elev_set_button_lamp(BUTTON_CALL_DOWN, floor, false);
            break;
        default:
            elev_set_button_lamp(BUTTON_CALL_UP, floor, false);
            elev_set_button_lamp(BUTTON_CALL_DOWN, floor, false);
            break;
    }
}

void ElevatorFSM::buttonPressed(elev_button_type_t buttonType, int floor) {

    	orders->add(buttonType, floor);
        elev_set_button_lamp(buttonType, floor, true);

}
void ElevatorFSM::stopButtonPressed(void) {
    //std::cout << *orders << std::endl;
    orders->print();
}
void ElevatorFSM::sensorActivated(int floor) {
	elev_set_floor_indicator(floor);
    //DELME DEBUG
	orders->remove(BUTTON_CALL_UP, floor);
	orders->remove(BUTTON_CALL_DOWN, floor);
	resetFloorLights(floor);
    //ENF DEBUG
}