#pragma once

#include "elev.h"
#include "OrderList.hpp"
typedef enum state {
    IDLE, RUNNING, DOOR_OPEN, UNINITIALIZED
} state_t;
enum toggle {
	OFF, ON
};
class ElevatorFSM {
    private:
    	OrderList *orders;
    	int currentFloor;
    	int destination;
    	state_t elevatorState;
    	void setState(state_t nextState);
    	void resetFloorLights(int floor);
    public:
    	ElevatorFSM(OrderList* elevator);
		void buttonPressed(elev_button_type_t buttonType, int floor);
		void stopButtonPressed(void);
		void sensorActivated(int floor);
};