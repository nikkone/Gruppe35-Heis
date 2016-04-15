#pragma once
#include <boost/asio/ip/address_v4.hpp>

#include "elev.h"
#include "OrderList.hpp"
#include "Timer.hpp"
#include "ElevatorMap.hpp"
#include "Communication.hpp"

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
    	Timer doorTimer;
        Timer* motorTimer;

    	state_t elevatorState;
        ElevatorMap* elevators;
        Communication* com;

    	void setState(state_t nextState);
    	void resetFloorLights(int floor);
    	elev_motor_direction_t findDirection() const;
    	void TimerTimedOut();
		bool shouldStop(int floor);
        void interpretMessage(const address_v4 &messageIP, message_t messageType, int floor);
        //void interpretMessage(elev_button_type_t buttonType, int floor);

    public:

    	ElevatorFSM(OrderList* orderList_p, ElevatorMap* elevatorMap_p, Timer* motorTimer_p, Communication* com_p);
		void buttonPressed(elev_button_type_t buttonType, int floor);
		void floorSensorActivated(int floor);
		void stopButtonPressed(void);// FOR DEBUG
        void newDestination(int floor);
        void newMessages(const std::vector<std::tuple<address_v4, message_t, int>> &mail);
        void newMessages(const std::vector<std::tuple<elev_button_type_t, int>> &mail);
};
