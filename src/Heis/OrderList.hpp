#pragma once
#include <list>
#include <boost/asio/ip/address_v4.hpp>
#include "Button.hpp"
#include "ElevatorMap.hpp"
#include "elev.h"

using boost::asio::ip::address_v4;
class OrderList {
    private:
        std::list<Button> orders;
        std::list<Button>::iterator find(elev_button_type_t type, int floor);
    public:
    	bool checkOrder(elev_button_type_t type, int floor);
        void add(elev_button_type_t type, int floor);
        void remove(elev_button_type_t type, int floor);
        int getNextFloor(const address_v4 &ip, const ElevatorMap &elevators) const;
        friend std::ostream &operator<<( std::ostream &output, const OrderList &OrderList_ref );
};
