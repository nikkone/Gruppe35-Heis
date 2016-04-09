#include "OrderList.hpp"
#include <iostream>
void OrderList::add(elev_button_type_t type, int floor) {
    if(find(type, floor)==orders.end()) {
        orders.push_back(Button(type,floor));
    }
}
void OrderList::print() {//endre til stream
    for(std::list<Button>::iterator it = orders.begin(); it != orders.end(); it++) {
        std::cout << *it;
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
int OrderList::getNextFloor(ElevatorMap *elevators) {
    if(!orders.empty()) {
        int nextOrder;
        for(std::list<Button>::iterator button = orders.begin(); button != orders.end(); button++) {
            nextOrder = button->getFloor();
            if(button->getType() == BUTTON_COMMAND) {//Utfør intern kommando uansett
                return nextOrder;
            } else {//Kun for opp og ned knapper
                if(elevators->checkDestination(nextOrder) || elevators->checkLocation(nextOrder)) {
                    continue;
                } else {
                    if(elevators->shouldTakeOrder(nextOrder)) {
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
bool OrderList::checkOrder(elev_button_type_t type, int floor) {
    if(find(type, floor)==orders.end()) {
        return false;
    }
    return true;
}
/*
std::ostream &operator<<( std::ostream &output, const OrderList &OrderList_ref ) {
    for(std::list<Button>::iterator it = (OrderList_ref.orders).begin(); it != OrderList_ref.orders.end(); it++) {
        output << *it;
    }
    std::list<Button>::iterator it = (OrderList_ref.orders).begin();
    //std::list<Button>::iterator it = (OrderList_ref.orders).begin()
    return output;
}
*/
