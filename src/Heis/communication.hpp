#pragma once
#include "network.hpp"
#include <string.h>
#include "elev.h"
#include "fsm.hpp"
typedef enum msg_types {
    CALL_UP, CALL_DOWN, COMMAND, CURRENT_LOCATION, DESTINATION
} message_t;
class communication {
	private:
		network *com;
		ElevatorFSM *fsm;
		std::string ip;
		void decodeJSON(std::string json);
		std::string toJSON(message_t type, std::string content);
		char* findmyip();
	public:
		std::string getIP();
		communication(ElevatorFSM &inputFsm);
		void checkMailbox();
		void sendMail(message_t type, std::string content);
		void sendMail(elev_button_type_t buttonType, int floor);
};
