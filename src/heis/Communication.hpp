#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>

#include "elev.h"
#include "Network.hpp"
#include "ElevatorMap.hpp"

using boost::asio::ip::address_v4;

typedef enum msg_types {
    CALL_UP, CALL_DOWN, COMMAND, CURRENT_LOCATION, DESTINATION, SENDMEALL, FAILED
} message_t;

class Communication {
	
	private:
		Network network;

		std::tuple<address_v4, message_t, int> readJSON(std::string json);
		std::string makeJSON(message_t type, int floor);
		address_v4 findMyIP();

	public:
		Communication() : network(Network(8001, findMyIP())) {}; 
		const address_v4 getMyIP() const;
		const std::vector<std::tuple<address_v4, message_t, int>> checkMailbox();
		void updateElevatorMap(ElevatorMap &elevators);
		void sendMail(message_t type, int floor);
		void sendMail(elev_button_type_t buttonType, int floor);
};
