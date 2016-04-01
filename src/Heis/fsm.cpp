#include "fsm.hpp"

//for cout
#include <iostream>

ElevatorFSM::ElevatorFSM(OrderList *elevator) {
	orders = elevator;
}
 void resetFloorLights(int floor) {
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