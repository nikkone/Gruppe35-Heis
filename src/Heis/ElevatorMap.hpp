#pragma once
#include "Elevator.hpp"
#include <map>
#include <string>
// FOR DEBUGGIN
#include <iostream>
class ElevatorMap {
	private:
		std::map<std::string,  Elevator> elevatorMap;
	public:
	ElevatorMap();
	void addElevator(std::string, int location);
	//DEBUG
	void print();
};
