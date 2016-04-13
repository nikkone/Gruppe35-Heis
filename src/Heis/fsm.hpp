#pragma once
#include <boost/asio/ip/address_v4.hpp>

#include "elev.h"
#include "OrderList.hpp"
#include "timer.hpp"
#include "ElevatorMap.hpp"
#include "communication.hpp"

using boost::asio::ip::address_v4;

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
        Timer* motorTimer;

    	state_t elevatorState;
        ElevatorMap* elevators;
        communication* com;

    	void setState(state_t nextState);
    	void resetFloorLights(int floor);
    	elev_motor_direction_t findDirection();
    	void TimerTimedOut();
		bool stopCheck(int floor);
        void interpretMessage(address_v4 messageIP, message_t messageType, int floor);

    public:

    	ElevatorFSM(OrderList* orderList_p, ElevatorMap* elevatorMap_p, Timer* motorTimer_p, communication* com_p);
        ~ElevatorFSM();
		void buttonPressed(elev_button_type_t buttonType, int floor);
		void floorSensorActivated(int floor);
		void stopButtonPressed(void);// FOR DEBUG
        void newDestination(int floor);
        void newMail(std::vector<std::tuple<address_v4, message_t, int>>);
};
