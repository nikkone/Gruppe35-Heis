#pragma once
#include <string>
#include <tuple>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>
#include "network.hpp"
#include "elev.h"
#include "ElevatorMap.hpp"

using boost::asio::ip::address_v4;
typedef enum msg_types {
    CALL_UP, CALL_DOWN, COMMAND, CURRENT_LOCATION, DESTINATION, SENDMEALL, FAILED
} message_t;
class communication {
	private:
		network *com;//Changename
		ElevatorMap *elevators;

		address_v4 myIP;
		std::tuple<address_v4, message_t, int> decodeJSON(std::string json);
		//void interpretMessage(address_v4 messageIP, message_t messageType, int floor);
		std::string toJSON(message_t type, int floor);
		char* findmyip();
	public:
		address_v4 getMyIP();
		communication(ElevatorMap *elevators_p);
		~communication();

		std::vector<std::tuple<address_v4, message_t, int>> checkMailbox();
		void updateElevatorMap();
		void sendMail(message_t type, int floor);
		void sendMail(elev_button_type_t buttonType, int floor);
};
