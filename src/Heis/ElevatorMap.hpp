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

		address_v4 first;

	public:
	ElevatorMap();
	void addElevator(address_v4 ip, int location);
	void addElevator(address_v4 ip);
	void removeElevator(address_v4 ip);
	void setDestination(address_v4 ip, int destination);
	void setDestination(int destination);
	void setCurrentLocation(address_v4 ip, int location);
	void setCurrentLocation(int location);
	int getDestination(address_v4 ip);
	int getDestination();
	int getCurrentLocation(address_v4 ip);
	int getCurrentLocation();
	bool checkDestination(int floor);
	bool checkLocation(int floor);
	//bool isNearest(int targetFloor);
	bool shouldTakeOrder(int order);
	//DEBUG
	void print();
};
