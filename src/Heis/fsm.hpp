#pragma once

#include "elev.h"
#include "OrderList.hpp"
#include "timer.hpp"
////////////////////////////      TODO       ///////////////////////////////
/*
	- LEGGE ALLE ENDRINGER UT PÅ NETTET
	- Kostfunksjon for flere heiser
*/
///////////////////////////////////////////////////////////////////////////
typedef enum state {
    IDLE, RUNNING, DOOR_OPEN, UNINITIALIZED
} state_t;
enum toggle {
	OFF, ON
};
class ElevatorFSM {
    private:
    	OrderList* orders;
    	Timer* timer;
    	int currentFloor;
    	int destination;
    	state_t elevatorState;

    	void setState(state_t nextState);
    	void resetFloorLights(int floor);
    	elev_motor_direction_t findDirection();
    	void TimerTimedOut();
		void setNewDestination(int newDest);
		bool stopCheck(int floor);
    public:
    	ElevatorFSM(OrderList* elevator);

		void buttonPressed(elev_button_type_t buttonType, int floor);
		void sensorActivated(int floor);

		void stopButtonPressed(void);// FOR DEBUG

};