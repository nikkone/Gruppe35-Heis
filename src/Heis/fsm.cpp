#include "fsm.hpp"

//for cout
#include <iostream>

ElevatorFSM::ElevatorFSM(ElevatorListEntry *elevator) {
	localElevator = elevator;
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

	localElevator->setFloor(floor, buttonType);
    elev_set_button_lamp(buttonType, floor, true);

}
void ElevatorFSM::stopButtonPressed(void) {
    std::cout << *localElevator << std::endl;
}
void ElevatorFSM::sensorActivated(int floor) {
	elev_set_floor_indicator(floor);
    //DELME DEBUG
	localElevator->resetFloor(floor, DIRN_UP);
	localElevator->resetFloor(floor, DIRN_DOWN);
	resetFloorLights(floor);
    //ENF DEBUG
}