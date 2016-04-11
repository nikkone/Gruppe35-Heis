#include "fsm.hpp"

//for cout
#include <iostream>

ElevatorFSM::ElevatorFSM(OrderList* orderList_p, ElevatorMap* elevatorMap_p) {
    elevators = elevatorMap_p;
	orders = orderList_p;
    setState(UNINITIALIZED);
    timer = new Timer();
}
ElevatorFSM::~ElevatorFSM() {
    delete timer;
}
void ElevatorFSM::TimerTimedOut() {
    setState(IDLE);
}
void ElevatorFSM::setState(state_t nextState) {
    //std::cout <<"changestate: " << nextState << std::endl;
    elevatorState = nextState;
    switch (nextState) {
        case IDLE:
            elev_set_motor_direction(DIRN_STOP);
            elev_set_door_open_lamp(OFF);
            break;
        case MOVING:
            elev_set_door_open_lamp(OFF);
            elev_set_motor_direction(findDirection());
            break;
        case DOOR_OPEN:
            elev_set_motor_direction(DIRN_STOP);
            elev_set_door_open_lamp(ON);
            elevators->setDestination(-1);
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

elev_motor_direction_t ElevatorFSM::findDirection() {
    int direction = elevators->getDestination() - elevators->getCurrentLocation();
    if(direction > 0) {
        return DIRN_UP;
    } else if(direction < 0) {
        return DIRN_DOWN;
    } else {
        return DIRN_STOP;
    }
}


void ElevatorFSM::buttonPressed(elev_button_type_t buttonType, int floor) {
    orders->add(buttonType, floor);
    elev_set_button_lamp(buttonType, floor, ON);
}
void ElevatorFSM::stopButtonPressed(void) {
    elevators->print();
}
bool ElevatorFSM::stopCheck(int floor) {
    if(orders->checkOrder(BUTTON_COMMAND, floor)) {
        return true;
    }
    //Sjekker om etasjen er min destinasjon
    if(floor==elevators->getDestination()) {
        return true;
    }
    //Sjekker om etasjen er betjent av noen andre
    if(elevators->checkDestination(floor)) {
        return false;
    }
    //Sjekker om andre heiser er i etasjen
    if(elevators->checkLocation(floor)) {
        return false;
    }

    switch(findDirection()) {
        case DIRN_UP:
            if(orders->checkOrder(BUTTON_CALL_UP, floor)) {
                return true;
            }
            break;
        case DIRN_DOWN:
            if(orders->checkOrder(BUTTON_CALL_DOWN, floor)) {
                return true;
            }
            break;
        case DIRN_STOP:
            return true;
            break;
    }
    return false;
}
void ElevatorFSM::floorSensorActivated(int floor) {

    if((floor != elevators->getCurrentLocation()) || (floor == elevators->getDestination())) {
        elevators->setCurrentLocation(floor);
    	elev_set_floor_indicator(floor);
        if(stopCheck(floor)) {
            setState(DOOR_OPEN);
        	orders->remove(BUTTON_CALL_UP, floor);
        	orders->remove(BUTTON_CALL_DOWN, floor);
            orders->remove(BUTTON_COMMAND, floor);
        	resetFloorLights(floor);
        }
    }

    //Poll timer, bør kun gjøres i state DOOR_OPEN
    if(timer->check()) {
        TimerTimedOut();
    }
}
void ElevatorFSM::newDestination(int floor) {
    if(elevatorState == IDLE) {
        elevators->setDestination(floor);
        setState(MOVING);
    }/* else if(elevatorState == MOVING) {
        setState(MOVING);
    }*/
}
