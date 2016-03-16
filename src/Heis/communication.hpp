#pragma once
#include "network.hpp"
#include <string.h>
typedef enum msg_types {
    BUTTON_COMMAND, BUTTON_CALL_UP, BUTTON_CALL_DOWN
} message_t;
class communication {
	private:
		network *com;
		std::string ip;
	public:
		communication();
		char* findmyip();
		std::string toJSON(message_t type, std::string content);
		void decodeJSON(std::string json);
};
