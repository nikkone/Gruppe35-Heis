#include "ElevatorFSM.hpp"
#include <iostream>

enum toggle {
    OFF, ON
};

ElevatorFSM::ElevatorFSM(OrderList* orderList_p, ElevatorMap* elevatorMap_p, Timer* motorTimer_p,  Communication* commmunication_p) 
 : orders(orderList_p), motorTimer(motorTimer_p), elevators(elevatorMap_p), communication(commmunication_p) {
    setState(UNINITIALIZED);
}

void ElevatorFSM::doorTimerTimedOut() {
    setState(IDLE);
}

void ElevatorFSM::setState(state_t nextState) {
    elevatorState = nextState;
    switch (nextState) {
        case IDLE:
            elev_set_motor_direction(DIRN_STOP);
            elev_set_door_open_lamp(OFF);
            break;
        case MOVING:
            elev_set_door_open_lamp(OFF);
            elev_set_motor_direction(findDirection());
            motorTimer->set(5);
            break;
        case DOOR_OPEN:
            elev_set_motor_direction(DIRN_STOP);
            elev_set_door_open_lamp(ON);
            motorTimer->reset();
            elevators->setDestination(communication->getMyIP(), -1);
            doorTimer.set(2);
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

elev_motor_direction_t ElevatorFSM::findDirection() const {
    int direction = elevators->getDestination(communication->getMyIP()) - elevators->getCurrentLocation(communication->getMyIP());
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

bool ElevatorFSM::shouldStop(int floor) {
    if(orders->checkOrder(BUTTON_COMMAND, floor)) {
        return true;
    }
    if(floor==elevators->getDestination(communication->getMyIP())) {
        return true;
    }
    if(elevators->checkDestination(floor)) {
        return false;
    }
    if(elevators->checkLocation(communication->getMyIP(), floor)) {
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
    if((elevatorState == MOVING) && (floor != elevators->getCurrentLocation(communication->getMyIP()))) {
        motorTimer->set(5);
    }
    if((floor != elevators->getCurrentLocation(communication->getMyIP())) || (floor == elevators->getDestination(communication->getMyIP()))) {
        elevators->setCurrentLocation(communication->getMyIP(), floor);
    	elev_set_floor_indicator(floor);
        if(shouldStop(floor)) {
            setState(DOOR_OPEN);
        	orders->remove(BUTTON_CALL_UP, floor);
        	orders->remove(BUTTON_CALL_DOWN, floor);
            orders->remove(BUTTON_COMMAND, floor);
        	resetFloorLights(floor);
        }
    }
    if( (elevatorState == DOOR_OPEN) && (doorTimer.check())) {
        doorTimerTimedOut();
    }
}

void ElevatorFSM::newDestination(int floor) {
    if(elevatorState == IDLE) {
        elevators->setDestination(communication->getMyIP(), floor);
        setState(MOVING);
    }
}

void ElevatorFSM::interpretMessage(const address_v4 &messageIP, message_t messageType, int floor) {
        switch(messageType) {
            case COMMAND:
                buttonPressed(BUTTON_COMMAND, floor);
                break;
            case CALL_UP:
                buttonPressed(BUTTON_CALL_UP, floor);
                break;
            case CALL_DOWN:
                buttonPressed(BUTTON_CALL_DOWN, floor);
                break;  
            case CURRENT_LOCATION:
                elevators->setCurrentLocation(messageIP, floor);
                if( (floor == elevators->getDestination(messageIP)) && (floor != -1) ) {

                    if(floor!=0) {
                        elev_set_button_lamp(BUTTON_CALL_DOWN, floor, OFF);
                        orders->remove(BUTTON_CALL_DOWN, floor);
                    }

                    if(floor!=N_FLOORS-1) {
                        elev_set_button_lamp(BUTTON_CALL_UP, floor, OFF);
                        orders->remove(BUTTON_CALL_UP, floor);
                    }
                }
                break;
            case DESTINATION:
                if(elevators->getDestination(messageIP) != -1 && floor == -1) {
                    
                    if(elevators->getDestination(messageIP)!=0) {
                        elev_set_button_lamp(BUTTON_CALL_DOWN, elevators->getDestination(messageIP), OFF);
                        orders->remove(BUTTON_CALL_DOWN, elevators->getDestination(messageIP));
                    }

                    if(elevators->getDestination(messageIP)!=N_FLOORS-1) {
                        elev_set_button_lamp(BUTTON_CALL_UP, elevators->getDestination(messageIP), OFF);
                        orders->remove(BUTTON_CALL_UP, elevators->getDestination(messageIP));
                    }
                    
                }
                elevators->setDestination(messageIP, floor);
                break;
                
            case SENDMEALL:
                communication->sendMail(CURRENT_LOCATION, elevators->getCurrentLocation(communication->getMyIP()));
                communication->sendMail(DESTINATION, elevators->getDestination(communication->getMyIP()));
                for(int floor = 0; floor < N_FLOORS; ++floor){
                    for(int button = 0; button < N_BUTTONS-1; ++button){
                        if(button==BUTTON_CALL_DOWN && floor==0) continue;
                        if(button==BUTTON_CALL_UP && floor==N_FLOORS-1) continue;
                        if(orders->checkOrder((elev_button_type_t)button, floor)) {
                            communication->sendMail((elev_button_type_t)button, floor);
                        }
                    }
                }
                break;
            case FAILED:
                break;
    }
}

void ElevatorFSM::newMessages(const std::vector<std::tuple<address_v4, message_t, int>> &mail){
    for(std::vector<std::tuple<address_v4, message_t, int>>::const_iterator it = mail.begin(); it != mail.end(); ++it) {
        interpretMessage(std::get<0>(*it),std::get<1>(*it),std::get<2>(*it));   
    }
}

void ElevatorFSM::newMessages(const std::vector<std::tuple<elev_button_type_t, int>> &mail){
    for(std::vector<std::tuple<elev_button_type_t, int>>::const_iterator it = mail.begin(); it != mail.end(); ++it) {
        buttonPressed(std::get<0>(*it),std::get<1>(*it));   
    }
}