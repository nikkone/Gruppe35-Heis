#pragma once
#include <ostream>
#include "ButtonList.hpp"
#include "elev.h"
class ElevatorListEntry {
    private:
        //boost::asio::ip::tcp::socket_ptr *socket;
        int previousFloorVisited;
        int Destination;
        ButtonList *elevatorButtons_p;
        ButtonList *upButtons_p;
        ButtonList *downButtons_p;
    public:
        ElevatorListEntry(int floors);
        ~ElevatorListEntry();
        friend std::ostream &operator<<( std::ostream &output, const ElevatorListEntry &ElevatorListEntry_ref );
        void setFloor(int floor, elev_button_type_t button_type);
        void resetFloor(int floor, elev_motor_direction_t direction);
        bool checkFloor(int floor, elev_motor_direction_t direction);
        void resetElevator();//Tror egentlig vi ikke trenger denne, stop knappen kan ignoreres
};

