#pragma once
#include "network.hpp"
#include <string>
#include "elev.h"
#include "fsm.hpp"
#include "ElevatorMap.hpp"
#include "OrderList.hpp"
typedef enum msg_types {
    CALL_UP, CALL_DOWN, COMMAND, CURRENT_LOCATION, DESTINATION, SENDMEALL
} message_t;
class communication {
	private:
		network *com;//Changename
		ElevatorFSM *fsm;
		ElevatorMap *elevators;
		OrderList *orders;
		std::string ip;
		void decodeJSON(std::string messageIP, std::string json);
		std::string toJSON(message_t type, std::string content);
		char* findmyip();
	public:
		std::string getIP();
		communication(ElevatorFSM *inputFsm, ElevatorMap *elevators_p, OrderList *orders_p);
		void checkMailbox();
		void sendMail(message_t type, int content);
		void sendMail(elev_button_type_t buttonType, int floor);
};
