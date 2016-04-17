#pragma once
#include <ostream>

class Elevator {
	
    public:
        int currentLocation;
        int destination;

        Elevator() : currentLocation(0), destination(-1) {};
        Elevator(int dest, int loc) : currentLocation(loc), destination(dest) {};
        friend std::ostream &operator<<( std::ostream &output, const Elevator &Elevator_ref );
};
