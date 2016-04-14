#include "ElevatorMap.hpp"
#include <vector>


ElevatorMap::ElevatorMap() {
}




void ElevatorMap::addElevator(const address_v4 &ip, int location) {
	if(elevatorMap.find(ip) == elevatorMap.end()) {
		//ip = ip.substr(0,15);
		std::cout << ip << " added" << std::endl;
		elevatorMap[ip] = Elevator(-1,location);
	}
}
void ElevatorMap::addElevator(const address_v4 &ip) {
	addElevator(ip, -1);
}
void ElevatorMap::removeElevator(const address_v4 &ip) {
	//ip = ip.substr(0,15);
	std::cout << "Deleting ->";
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		elevatorMap.erase(ip);
		std::cout << "Deleted\n";
	}
}


void ElevatorMap::setCurrentLocation(const address_v4 &ip, int location) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		elevatorMap[ip].currentLocation = location;
	}
}
void ElevatorMap::setDestination(const address_v4 &ip, int destination) {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		elevatorMap[ip].destination = destination;
	}
}

int ElevatorMap::getDestination(const address_v4 &ip) const {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		//return elevatorMap[ip].destination;
		return (elevatorMap.find(ip)->second).destination;
	}
	return -1;
}
int ElevatorMap::getCurrentLocation(const address_v4 &ip) const {
	if(elevatorMap.find(ip) != elevatorMap.end()) {
		//return elevatorMap[ip].currentLocation;
		return (elevatorMap.find(ip)->second).currentLocation;
	}
	return -1;
}
bool ElevatorMap::checkDestination(int floor) const {
	for(std::map<address_v4,  Elevator>::const_iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
		if((it->second).destination == floor) {
			return true;
		}
    }
    return false;
}
bool ElevatorMap::checkLocation(const address_v4 &ip, int floor) const {
	for(std::map<address_v4,  Elevator>::const_iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
		if((it->second).currentLocation == floor) {
			if(it->first != ip) {
				return true;
			}
		}
    }
    return false;
}
bool ElevatorMap::shouldTakeOrder(const address_v4 &ip, int order) const {
	int myDistance = abs(getCurrentLocation(ip) - order);
	std::vector<address_v4> equidistantElevators;
	for(std::map<address_v4,  Elevator>::const_iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
		if((it->second).destination == -1) {
			if( (abs((it->second).currentLocation - order) < myDistance) )  {
				return false;
			} else if( abs( (it->second).currentLocation - order) == myDistance ) {
				equidistantElevators.push_back(it->first);
			}
		}
    }
    for(std::vector<address_v4>::iterator it = equidistantElevators.begin(); it != equidistantElevators.end(); it++) {
    	if(*it < ip) {
    		return false;
    	}
    }
    return true;
}

std::ostream &operator<<( std::ostream &output, const ElevatorMap &ElevatorMap_ref ) {
    for(std::map<address_v4,  Elevator>::const_iterator it = (ElevatorMap_ref.elevatorMap).begin(); it != (ElevatorMap_ref.elevatorMap).end(); it++) {
        output << "\"" << it->first << "\" : " << it->second;
    }
    output << "--------------------------" << std::endl;
    return output;
}
