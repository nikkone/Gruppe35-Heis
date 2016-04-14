#include "fsm.hpp"

//for cout
#include <iostream>

ElevatorFSM::ElevatorFSM(OrderList* orderList_p, ElevatorMap* elevatorMap_p, Timer* motorTimer_p,  communication* com_p) {
    elevators = elevatorMap_p;
	orders = orderList_p;
    motorTimer = motorTimer_p;
    com = com_p;
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
            motorTimer->set(5);
            break;
        case DOOR_OPEN:
            elev_set_motor_direction(DIRN_STOP);
            elev_set_door_open_lamp(ON);
            motorTimer->reset();
            elevators->setDestination(com->getMyIP(), -1);
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
    int direction = elevators->getDestination(com->getMyIP()) - elevators->getCurrentLocation(com->getMyIP());
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
    if(floor==elevators->getDestination(com->getMyIP())) {
        return true;
    }
    //Sjekker om etasjen er betjent av noen andre
    if(elevators->checkDestination(floor)) {
        return false;
    }
    //Sjekker om andre heiser er i etasjen
    if(elevators->checkLocation(com->getMyIP(), floor)) {
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
    if((elevatorState == MOVING) && (floor != elevators->getCurrentLocation(com->getMyIP()))) {
        motorTimer->set(5);
    }
    if((floor != elevators->getCurrentLocation(com->getMyIP())) || (floor == elevators->getDestination(com->getMyIP()))) {
        elevators->setCurrentLocation(com->getMyIP(), floor);
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
        elevators->setDestination(com->getMyIP(), floor);
        setState(MOVING);
    }/* else if(elevatorState == MOVING) {
        setState(MOVING);
    }*/
}

void ElevatorFSM::interpretMessage(address_v4 messageIP, message_t messageType, int floor) {
        switch(messageType) {
            case COMMAND:
                std::cout << "COMMAND" << floor << std::endl;
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
                //FOR DEBUG
                std::cout << "Ip:" << messageIP << "Arrived at: " << floor << std::endl;
                break;
            case DESTINATION:
            //SE GJWENNOM
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
                //FOR DEBUG
                std::cout << "Ip:" << messageIP << "Going to: " << floor << std::endl;
                break;
                
            case SENDMEALL:
                com->sendMail(CURRENT_LOCATION, elevators->getCurrentLocation(com->getMyIP()));
                com->sendMail(DESTINATION, elevators->getDestination(com->getMyIP()));
                for(int floor = 0; floor < N_FLOORS; floor++){
                    for(int button = 0; button < N_BUTTONS-1; button++){
                        if(button==BUTTON_CALL_DOWN && floor==0) continue;
                        if(button==BUTTON_CALL_UP && floor==N_FLOORS-1) continue;
                        if(orders->checkOrder((elev_button_type_t)button, floor)) {
                            com->sendMail((elev_button_type_t)button, floor);
                        }
                    }
                }
                break;
            case FAILED:
                break;
    }
}
void ElevatorFSM::newMail(std::vector<std::tuple<address_v4, message_t, int>> mail){
    for(std::vector<std::tuple<address_v4, message_t, int>>::iterator it = mail.begin(); it != mail.end(); it++) {
        interpretMessage(std::get<0>(*it),std::get<1>(*it),std::get<2>(*it));   
    }
}