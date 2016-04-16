#pragma once
#include <ostream>

class Elevator {
    public:
        int currentLocation;
        int destination;
        Elevator();
        Elevator(int dest, int loc);
        friend std::ostream &operator<<( std::ostream &output, const Elevator &Elevator_ref );
};
