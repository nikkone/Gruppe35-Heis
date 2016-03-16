#include "fsm.hpp"

//for cout
#include <iostream>

ElevatorFSM::ElevatorFSM(ElevatorListEntry *elevator) {
	localElevator = elevator;
}
void ElevatorFSM::buttonPressed(int floor, elev_button_type_t buttonType) {
	localElevator->setFloor(floor, buttonType);
    elev_set_button_lamp(buttonType, floor, true);

}
void ElevatorFSM::stopButtonPressed(void) {
    std::cout << *localElevator << std::endl;
}
void ElevatorFSM::sensorActivated(int floor) {
	elev_set_floor_indicator(floor);
}