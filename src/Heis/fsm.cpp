#include "fsm.hpp"

//for cout
#include <iostream>

ElevatorFSM::ElevatorFSM(OrderList* orderList_p, ElevatorMap* elevatorMap_p) {
    //Endre elevator navn, IKKE BRA!!!!!
    elevators = elevatorMap_p;
	orders = orderList_p;
    //destination = -1;
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
            elevators->setDestination(-1);
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
        //std::cout << "OPP" << std::endl;
        return DIRN_UP;
    } else if(direction < 0) {
        //std::cout << "NED" << std::endl;
        return DIRN_DOWN;
    } else {
        return DIRN_STOP;
    }
}


void ElevatorFSM::buttonPressed(elev_button_type_t buttonType, int floor) {
    if( !( ((elevatorState == IDLE) || (elevatorState == DOOR_OPEN)) && (floor == elevators->getCurrentLocation()) ) ) {
    	orders->add(buttonType, floor);
        elev_set_button_lamp(buttonType, floor, ON);
    } else {
        setState(DOOR_OPEN);
    }
}
void ElevatorFSM::stopButtonPressed(void) {
    //std::cout << *orders << std::endl;
    elevators->print();
    //elevators
}
bool ElevatorFSM::stopCheck(int floor) {
    //Sjekker om etasjen er min destinasjon
    if(floor==elevators->getDestination()) {
        return true;
    }
    //Sjekker om etasjen er betjent av noen andre
    if(elevators->checkDestination(floor)) {
        return false;
    }
    if(orders->exists(BUTTON_COMMAND, floor)) {
        return true;
    }
    switch(findDirection()) {
        case DIRN_UP:
            if(orders->exists(BUTTON_CALL_UP, floor)) {
                return true;
            }
            break;
        case DIRN_DOWN:
            if(orders->exists(BUTTON_CALL_DOWN, floor)) {
                return true;
            }
            break;
        case DIRN_STOP:
            return true;
            break;
    }
    return false;
}
void ElevatorFSM::sensorActivated(int floor) {
    if(floor != elevators->getCurrentLocation()) {
        elevators->setCurrentLocation(floor);
    	elev_set_floor_indicator(floor);
        //DELME DEBUG
        //std::cout << floor << std::endl;
        if(stopCheck(floor)) {
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
        //HUSK Å SENDE DETTE PÅ NETTET
        elevators->setDestination(newDest);
        setState(RUNNING);
    }
}