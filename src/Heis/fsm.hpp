#pragma once

#include "elev.h"
#include "OrderList.hpp"
#include "timer.hpp"
#include "ElevatorMap.hpp"

typedef enum state {
    IDLE, MOVING, DOOR_OPEN, UNINITIALIZED
} state_t;
enum toggle {
	OFF, ON
};

const int N_BUTTONS = 3;
class ElevatorFSM {
    private:
    	OrderList* orders;
    	Timer* timer;
    	state_t elevatorState;
        ElevatorMap* elevators;
    	void setState(state_t nextState);
    	void resetFloorLights(int floor);
    	elev_motor_direction_t findDirection();
    	void TimerTimedOut();
		void setNewDestination(int floor);
		bool stopCheck(int floor);
    public:

    	ElevatorFSM(OrderList* orderList_p, ElevatorMap* elevatorMap_p);

		void buttonPressed(elev_button_type_t buttonType, int floor);
		void floorSensorActivated(int floor);

		void stopButtonPressed(void);// FOR DEBUG

};