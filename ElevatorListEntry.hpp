#ifndef __INCLUDE_ElevatorListEntry_HPP__
#define __INCLUDE_ElevatorListEntry_HPP__
#include <ostream>
#include "ButtonList.hpp"
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
        void setFloor(int floor, int button_type);
        void resetFloor(int floor, int direction);
        bool checkFloor(int floor, int direction);
        void resetElevator();
};
#endif //#ifndef __ElevatorListEntry_HPP__
