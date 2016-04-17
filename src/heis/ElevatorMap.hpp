#pragma once
#include <map>
#include <ostream>
#include <boost/asio/ip/address_v4.hpp>

#include "Elevator.hpp"

using boost::asio::ip::address_v4;

class ElevatorMap {
	private:
		std::map<address_v4,  Elevator> elevatorMap;
		
	public:
	void addElevator(const address_v4 &ip, int location);
	void addElevator(const address_v4 &ip);
	void removeElevator(const address_v4 &ip);
	void setDestination(const address_v4 &ip, int destination);
	void setCurrentLocation(const address_v4 &ip, int location);
	int getDestination(const address_v4 &ip) const;
	int getCurrentLocation(const address_v4 &ip) const;
	bool checkLocation(const address_v4 &ip, int floor) const;
	bool checkDestination(int floor) const;
	bool shouldTakeOrder(const address_v4 &ip, int order) const;

	friend std::ostream &operator<<( std::ostream &output, const ElevatorMap &ElevatorMap_ref );
};
