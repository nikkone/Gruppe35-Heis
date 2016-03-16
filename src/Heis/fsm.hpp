#pragma once

#include "elev.h"
#include "ElevatorListEntry.hpp"
class ElevatorFSM {
    private:
    	ElevatorListEntry *localElevator;
    public:
    	ElevatorFSM(ElevatorListEntry* elevator);
		void buttonPressed(int floor, elev_button_type_t buttonType);
		void stopButtonPressed(void);
		void sensorActivated(int floor);
};