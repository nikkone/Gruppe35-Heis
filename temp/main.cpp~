#include "OrderList.hpp"
#include <iostream>
#include <utility>
#include <map>
#include <string>
#include "Elevator.hpp"
int main() {
    //typedef std::pair <int,int> Elevator;
    //struct Elevator { int dest; int loc; };// Elevator;
    std::map<std::string,  Elevator> elevatorMap;
    elevatorMap["192.168.2.200"] = Elevator(1000,1);
    elevatorMap["192.168.2.200"].currentLocation = 5;
    std::cout << elevatorMap["192.168.2.200"] << std::endl;
/*
    OrderList orders;
    orders.add(BUTTON_COMMAND,7);
    orders.add(BUTTON_CALL_DOWN,4);
    orders.add(BUTTON_CALL_UP,3);
    orders.add(BUTTON_CALL_UP,4);
    orders.add(BUTTON_CALL_UP,3);
    orders.add(BUTTON_CALL_UP,3);

    orders.remove(BUTTON_CALL_UP,3);
    orders.remove(BUTTON_CALL_UP,3);
    orders.add(BUTTON_CALL_UP,43);
    orders.print();
    std::cout << orders.getNextFloor() << std::endl;
    std::cout << orders.getNextFloor() << std::endl;
*/
    return 0;
}
