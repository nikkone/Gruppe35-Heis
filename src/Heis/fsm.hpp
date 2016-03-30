#pragma once

#include "elev.h"
#include "ElevatorListEntry.hpp"
class ElevatorFSM {
    private:
    	ElevatorListEntry *localElevator;
    public:
    	ElevatorFSM(ElevatorListEntry* elevator);
		void buttonPressed(elev_button_type_t buttonType, int floor);
		void stopButtonPressed(void);
		void sensorActivated(int floor);
};