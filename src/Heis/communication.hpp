#pragma once
#include "network.hpp"
#include <string.h>
#include "elev.h"
typedef enum msg_types {
    COMMAND, CALL_UP, CALL_DOWN
} message_t;
class communication {
	private:
		network *com;
		std::string ip;
		void decodeJSON(std::string json);
		std::string toJSON(message_t type, std::string content);
		char* findmyip();
	public:
		communication();
		void checkMailbox();
		void sendMail(message_t type, std::string content);
		void sendMail(elev_button_type_t buttonType, int floor);
};
