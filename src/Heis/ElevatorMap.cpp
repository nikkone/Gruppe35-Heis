#include "ElevatorMap.hpp"

ElevatorMap::ElevatorMap() {
}


void ElevatorMap::addElevator(std::string ip, int location) {
	if(elevatorMap.empty()) {
		first = ip;
	}
	elevatorMap[ip] = Elevator(-1,location);
}
void ElevatorMap::addElevator(std::string ip) {
	addElevator(ip, -1);
}
void ElevatorMap::removeElevator(std::string ip) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		elevatorMap.erase(ip);
	}
}
void ElevatorMap::print() {//endre til stream
    for(std::map<std::string,  Elevator>::iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
        std::cout << "\"" << it->first << "\" : " << it->second;
    }
    std::cout <<"--------------------------" << std::endl;
}
void ElevatorMap::setCurrentLocation(std::string ip, int location) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		elevatorMap[ip].currentLocation = location;
	} else {
		addElevator(ip, location);
	}
}
void ElevatorMap::setCurrentLocation(int location) {
	setCurrentLocation(first, location);
}
void ElevatorMap::setDestination(std::string ip, int destination) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		elevatorMap[ip].destination = destination;
	} else {
		addElevator(ip);
		elevatorMap[ip].destination = destination;
	}
}
void ElevatorMap::setDestination(int destination) {
	setDestination(first, destination);
}
int ElevatorMap::getCurrentLocation(std::string ip) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		return elevatorMap[ip].currentLocation;
	}
	return -1;
}
int ElevatorMap::getCurrentLocation() {
	return getCurrentLocation(first);
}

int ElevatorMap::getDestination(std::string ip) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		return elevatorMap[ip].destination;
	}
	return -1;
}
int ElevatorMap::getDestination() {
	return getDestination(first);
}
bool ElevatorMap::checkDestination(int floor) {
	for(std::map<std::string,  Elevator>::iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
		if((it->second).destination == floor) {
			return true;
		}
    }
    return false;
}