#pragma once
#include <list>
#include "Button.hpp"
#include "ElevatorMap.hpp"
#include "elev.h"
class OrderList {
    private:
        std::list<Button> orders;
        std::list<Button>::iterator find(elev_button_type_t type, int floor);
    public:
    	bool checkOrder(elev_button_type_t type, int floor);
        void add(elev_button_type_t type, int floor);
        void remove(elev_button_type_t type, int floor);
        int getNextFloor(ElevatorMap *elevators);

        //FOR DEBUG
        //friend std::ostream &operator<<( std::ostream &output, const OrderList &OrderList_ref );
        void print();
};
