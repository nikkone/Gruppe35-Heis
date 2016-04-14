#include "OrderList.hpp"
#include <iostream>
void OrderList::add(elev_button_type_t type, int floor) {
    if(checkOrder(type, floor)) {
        orders.push_back(Button(type,floor));
    }
}

std::list<Button>::iterator OrderList::find(elev_button_type_t type, int floor) {
    for(std::list<Button>::iterator it = orders.begin(); it != orders.end(); it++) {
        if(it->getFloor()==floor) {
            if(it->getType()==type) {
                return it;
            }
        }
    }
    return orders.end();
}
void OrderList::remove(elev_button_type_t type, int floor) {
    std::list<Button>::iterator it = find(type, floor);
    if(it!=orders.end()) {
        orders.erase(it);
    }
}
int OrderList::getNextFloor(const address_v4 &ip, const ElevatorMap &elevators) const {
    if(!orders.empty()) {
        int nextOrder;
        for(std::list<Button>::const_iterator button = orders.begin(); button != orders.end(); button++) {
            nextOrder = button->getFloor();
            if(button->getType() == BUTTON_COMMAND) {//Utfør intern kommando uansett
                return nextOrder;
            } else {//Kun for opp og ned knapper
                if(elevators.checkDestination(nextOrder) || elevators.checkLocation(ip, nextOrder)) {
                    continue;
                } else {
                    if(elevators.shouldTakeOrder(ip, nextOrder)) {
                        return nextOrder;
                    } else {
                        continue;
                    }
                }
            }
        }
    }
    return -1;
}
bool OrderList::checkOrder(elev_button_type_t type, int floor) const {
    for(std::list<Button>::const_iterator it = orders.begin(); it != orders.end(); it++) {
        if(it->getFloor()==floor) {
            if(it->getType()==type) {
                return true;
            }
        }
    }
    return false;
}

std::ostream &operator<<( std::ostream &output, const OrderList &OrderList_ref ) {
    for(std::list<Button>::const_iterator it = (OrderList_ref.orders).begin(); it != OrderList_ref.orders.end(); it++) {
        output << *it;
    }
    return output;
}

