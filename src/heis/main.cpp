#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <boost/asio/ip/address_v4.hpp>

#include "elev.h"
#include "Timer.hpp"
#include "Backup.hpp"
#include "OrderList.hpp"
#include "ElevatorMap.hpp"
#include "ElevatorFSM.hpp"
#include "Communication.hpp"

using boost::asio::ip::address_v4;

const int backupInterval = 1; //seconds

int main() {

    int pid = fork();
    if(pid == 0){
        int spawned = execl("./bin/watchdog", "watchdog", "&", (char*)NULL);
        if(spawned == -1)
        {
          std::cerr << "Watchdog failed to start" << std::endl;
        }
    }

    OrderList orders;
    ElevatorMap elevators;
    Timer motorTimer;
    Communication communication;
    elevators.addElevator(communication.getMyIP(), 0);
    Backup backup("backup.txt");
    Timer backupTimer;
    backupTimer.set(backupInterval);
    ElevatorFSM fsm = ElevatorFSM(&orders, &elevators, &motorTimer, &communication);
    fsm.newMessages(backup.readBackup());
    int previousFloorSensor = -1;
    int previousDestination= -1;
    while(true) {
        fsm.newMessages(communication.checkMailbox());        
        communication.updateElevatorMap(elevators);

        if(previousDestination != elevators.getDestination(communication.getMyIP())) {
            previousDestination = elevators.getDestination(communication.getMyIP());
            communication.sendMail(DESTINATION, previousDestination);
        }

        int floorSensorSignal = elev_get_floor_sensor_signal();
        if(floorSensorSignal != -1) {
            if(floorSensorSignal != previousFloorSensor) {
                communication.sendMail(CURRENT_LOCATION, floorSensorSignal);
                previousFloorSensor = floorSensorSignal; 
            }
            fsm.floorSensorActivated(floorSensorSignal);
        }

        if(orders.getNextFloor(communication.getMyIP(), elevators) != -1) {
            fsm.newDestination(orders.getNextFloor(communication.getMyIP(), elevators));
        }

        static bool prev[N_FLOORS][N_BUTTONS];
        for(int floor = 0; floor < N_FLOORS; ++floor){
            for(int button = 0; button < N_BUTTONS; ++button){
                if(button==BUTTON_CALL_DOWN && floor==0) continue;
                if(button==BUTTON_CALL_UP && floor==N_FLOORS-1) continue;
                bool buttonSignal = elev_get_button_signal((elev_button_type_t)button, floor);
                if(buttonSignal  &&  buttonSignal != prev[floor][button]){
                    fsm.buttonPressed((elev_button_type_t)button, floor);
                    if(button!=BUTTON_COMMAND) {
                        communication.sendMail((elev_button_type_t)button, floor);
                    }
                }
                prev[floor][button] = buttonSignal;
            }
        }

        if(backupTimer.check()) {
            backup.writeBackup(orders);
            backupTimer.set(backupInterval);
        }

        if(motorTimer.check()) {
            std::cerr << "Elevator hardware timed out!" << std::endl;
            return 1;
        }
        usleep(100000);
    }

    return 0;
}