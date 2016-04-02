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

    return 0;
}
