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
int OrderList::getNextFloor() {
    if(!orders.empty()) {
        int temp = orders.front().getFloor();
        orders.pop_front();
        return temp;
    }
    return -1;
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
