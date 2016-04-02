#include "ElevatorMap.hpp"

ElevatorMap::ElevatorMap() {
}
void ElevatorMap::addElevator(std::string ip, int location) {
	elevatorMap[ip] = Elevator(-1,location);
}
void ElevatorMap::print() {//endre til stream
    for(std::map<std::string,  Elevator>::iterator it = elevatorMap.begin(); it != elevatorMap.end(); it++) {
        std::cout << it->second << std::endl;
    }
}