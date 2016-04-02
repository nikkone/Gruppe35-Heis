#include "fsm.hpp"

//for cout
#include <iostream>

ElevatorFSM::ElevatorFSM(OrderList *elevator) {
    //Endre elevator navn, IKKE BRA!!!!!
	orders = elevator;
    destination = -1;
    setState(UNINITIALIZED);
    timer = new Timer();
}
void ElevatorFSM::TimerTimedOut() {
    setState(IDLE);
}
void ElevatorFSM::setState(state_t nextState) {
    //std::cout << (int)nextState << std::endl;
    elevatorState = nextState;
    switch (nextState) {
        case IDLE:
            elev_set_motor_direction(DIRN_STOP);
            elev_set_stop_lamp(OFF);
            elev_set_door_open_lamp(OFF);
            break;
        case RUNNING:
            elev_set_motor_direction(findDirection());
            break;
        case DOOR_OPEN:
            elev_set_door_open_lamp(ON);
            destination = -1;
            elev_set_motor_direction(DIRN_STOP);
            timer->set(2);
            break;
        case UNINITIALIZED:
            if (!elev_init()) {
                exit(1);
            }
            elev_set_motor_direction(DIRN_UP);
            while(elev_get_floor_sensor_signal() == -1);
            setState(IDLE);
            break; 
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

elev_motor_direction_t ElevatorFSM::findDirection() {
    int direction = destination - currentFloor;
    if(direction > 0) {
        std::cout << "OPP" << std::endl;
        return DIRN_UP;
    } else if(direction < 0) {
        std::cout << "NED" << std::endl;
        return DIRN_DOWN;
    } else {
        return DIRN_STOP;
    }
}

void ElevatorFSM::buttonPressed(elev_button_type_t buttonType, int floor) {
    if( !( ((elevatorState == IDLE) || (elevatorState == DOOR_OPEN)) && (floor == currentFloor) ) ) {
    	orders->add(buttonType, floor);
        elev_set_button_lamp(buttonType, floor, true);
    } else {
        setState(DOOR_OPEN);
    }
}
void ElevatorFSM::stopButtonPressed(void) {
    //std::cout << *orders << std::endl;
    orders->print();
}
void ElevatorFSM::sensorActivated(int floor) {
    if(floor != currentFloor) {
        currentFloor = floor;
    	elev_set_floor_indicator(floor);
        //DELME DEBUG
        std::cout << floor << std::endl;
        if(floor==destination) {
            setState(DOOR_OPEN);
        	orders->remove(BUTTON_CALL_UP, floor);
        	orders->remove(BUTTON_CALL_DOWN, floor);
            orders->remove(BUTTON_COMMAND, floor);
        	resetFloorLights(floor);
        }
        //END DEBUG
    }
    //Check if any new orders
    if(orders->getNextFloor() != -1) {
        setNewDestination(orders->getNextFloor());
    }
    //Poll timer
    if(timer->check()) {
        TimerTimedOut();
    }
}
void ElevatorFSM::setNewDestination(int newDest) {
    if(elevatorState == IDLE) {
        destination = newDest;
        setState(RUNNING);
    }
}