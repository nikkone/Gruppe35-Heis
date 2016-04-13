#include "ElevatorMap.hpp"
#include <cstdlib>
#include <vector>


ElevatorMap::ElevatorMap() {
}


void ElevatorMap::addElevator(address_v4 ip, int location) {
	if(elevatorMap.empty()) {
		first = ip;
	}
	if(elevatorMap.find(ip) == elevatorMap.end()) {
		//ip = ip.substr(0,15);
		std::cout << ip << " added" << std::endl;
		elevatorMap[ip] = Elevator(-1,location);
	}
}
void ElevatorMap::addElevator(address_v4 ip) {
	addElevator(ip, -1);
}
void ElevatorMap::removeElevator(address_v4 ip) {
	//ip = ip.substr(0,15);
	std::cout << "Deleting ->";
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		elevatorMap.erase(ip);
		std::cout << "Deleted\n";
	}
}
void ElevatorMap::print() {//endre til stream
    for(std::map<address_v4,  Elevator>::iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
        std::cout << "\"" << it->first << "\" : " << it->second;
    }
    std::cout <<"--------------------------" << std::endl;
}
void ElevatorMap::setCurrentLocation(address_v4 ip, int location) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		elevatorMap[ip].currentLocation = location;
	}
}
/*
void ElevatorMap::setCurrentLocation(int location) {
	setCurrentLocation(first, location);
}
*/
void ElevatorMap::setDestination(address_v4 ip, int destination) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		elevatorMap[ip].destination = destination;
	}
}
/*
void ElevatorMap::setDestination(int destination) {
	setDestination(first, destination);
}
*/
int ElevatorMap::getCurrentLocation(address_v4 ip) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		return elevatorMap[ip].currentLocation;
	}
	return -1;
}
/*
int ElevatorMap::getCurrentLocation() {
	return getCurrentLocation(first);
}
*/
int ElevatorMap::getDestination(address_v4 ip) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		return elevatorMap[ip].destination;
	}
	return -1;
}
/*
int ElevatorMap::getDestination() {
	return getDestination(first);
}
*/
bool ElevatorMap::checkDestination(int floor) {
	for(std::map<address_v4,  Elevator>::iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
		if((it->second).destination == floor) {
			return true;
		}
    }
    return false;
}
bool ElevatorMap::checkLocation(int floor) {
	for(std::map<address_v4,  Elevator>::iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
		if((it->second).currentLocation == floor) {
			if(it->first != first) {
				return true;
			}
		}
    }
    return false;
}
bool ElevatorMap::shouldTakeOrder(int order) {
	int myDistance = abs(getCurrentLocation(first) - order);
	std::vector<address_v4> equidistantElevators;
	for(std::map<address_v4,  Elevator>::iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
		if((it->second).destination == -1) {
			if( (abs((it->second).currentLocation - order) < myDistance) )  {
				return false;
			} else if( abs( (it->second).currentLocation - order) == myDistance ) {
				equidistantElevators.push_back(it->first);
			}
		}
    }
    for(std::vector<address_v4>::iterator it = equidistantElevators.begin(); it != equidistantElevators.end(); it++) {
    	if(*it < first) {
    		return false;
    	}
    }
    return true;
}
