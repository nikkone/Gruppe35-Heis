
#include "ElevatorListEntry.hpp"
#include <vector>
#include <iostream>
#include <map>
int main() {
//Mulighet #2
    std::map<long, ElevatorListEntry(*)> elevatorList;
    elevatorList[192168001002]=new ElevatorListEntry(4);
    //elevatorList.erase(192168001002);
    elevatorList[192168001002]->setFloor(1,2);
    elevatorList[192168001002]->resetElevator();

    std::cout << elevatorList[192168001002]->checkFloor(1,2) << std::endl;
/*
    std::vector<ElevatorListEntry> elevatorList;
    elevatorList.push_back(ElevatorListEntry(4));
    elevatorList.push_back(ElevatorListEntry(4));
    elevatorList[0].setFloor(2, -1);
    elevatorList[0].setFloor(2, 1);
    elevatorList[0].setFloor(2, 0);
    //elevatorList[0].resetFloor(2, -1);
    std::cout << elevatorList[0] << std::endl;
    */
    return 0;
}

