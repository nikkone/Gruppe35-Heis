#pragma once
#include "Elevator.hpp"
#include <map>
#include <string>
// FOR DEBUGGIN
#include <iostream>
class ElevatorMap {
	private:
		std::map<std::string,  Elevator> elevatorMap;
		std::string first;
	public:
	ElevatorMap();
	void addElevator(std::string, int location);
	void setDestination(std::string ip, int destination);
	void setDestination(int destination);
	void setCurrentLocation(std::string ip, int location);
	void setCurrentLocation(int location);
	int getDestination(std::string ip);
	int getDestination();
	int getCurrentLocation(std::string ip);
	int getCurrentLocation();
	//DEBUG
	void print();
};