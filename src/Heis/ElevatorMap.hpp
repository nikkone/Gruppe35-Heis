#pragma once
#include "Elevator.hpp"
#include <map>
#include <string>
// FOR DEBUGGING
#include <iostream>

#include <boost/asio/ip/address_v4.hpp>
using boost::asio::ip::address_v4;

class ElevatorMap {
	private:
		std::map<address_v4,  Elevator> elevatorMap;
	public:
	ElevatorMap();
	void addElevator(address_v4 ip, int location);//Blir aldri brukt
	void addElevator(address_v4 ip);
	void removeElevator(address_v4 ip);
	void setDestination(address_v4 ip, int destination);
	void setCurrentLocation(address_v4 ip, int location);
	int getDestination(address_v4 ip);
	int getCurrentLocation(address_v4 ip);
	bool checkLocation(address_v4 ip, int floor);
	bool checkDestination(int floor);
	bool shouldTakeOrder(address_v4 ip, int order);

	//DEBUG
	void print();
};
