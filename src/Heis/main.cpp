
#include "elev.h"
#include "ElevatorListEntry.hpp"
#include "fsm.hpp"
#include "communication.hpp"

//for exit
#include <cstdlib>
//For sleep
#include <unistd.h>

#include <string.h>

//#include <vector>
#include <iostream>
#include <map>

int main() {

    //HW init
    if (!elev_init()) {
        std::exit(1);
    }
    elev_set_motor_direction(DIRN_UP);
    while(elev_get_floor_sensor_signal() == -1);
    elev_set_motor_direction(DIRN_STOP);

    //Orderlist init
    std::map<long, ElevatorListEntry(*)> elevatorList;
    elevatorList[192168001002] = new ElevatorListEntry(4);
    //FSM init
    ElevatorFSM fsm = ElevatorFSM(elevatorList[192168001002]);

    int sensorSignal;
    while(true) {
        sensorSignal = elev_get_floor_sensor_signal();
        if (sensorSignal >= 0) {
            fsm.sensorActivated(sensorSignal);
        }

        if(elev_get_stop_signal()){
            fsm.stopButtonPressed();
        } else {
            //fsm_stopButtonReleased();

            for (int floor = 0; floor < N_FLOORS; floor++) {
                if (elev_get_button_signal(BUTTON_COMMAND, floor)) {
                    fsm.buttonPressed(floor, BUTTON_COMMAND);                
                }
            }
            
            for (int floor = 1; floor < N_FLOORS; floor++) {
                if (elev_get_button_signal(BUTTON_CALL_DOWN, floor)) {
                    fsm.buttonPressed(floor, BUTTON_CALL_DOWN);
                }
            }

            for (int floor = 0; floor < N_FLOORS-1; floor++) {
                if (elev_get_button_signal(BUTTON_CALL_UP, floor)) {
                    fsm.buttonPressed(floor, BUTTON_CALL_UP);
                }
            }
            
        }
        usleep(100000);
    }
 /*   
//Mulighet #2
    std::map<long, ElevatorListEntry(*)> elevatorList;
    elevatorList[192168001002] = new ElevatorListEntry(4);
    //elevatorList.erase(192168001002);
    elevatorList[192168001002]->setFloor(1,2);
    //elevatorList[192168001002]->resetElevator();

    std::cout << *elevatorList[192168001002] << std::endl;

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

