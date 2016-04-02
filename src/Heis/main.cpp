
#include "elev.h"
#include "OrderList.hpp"
#include "fsm.hpp"
#include "communication.hpp"
#include "ElevatorMap.hpp"
//for exit
//#include <cstdlib>
//#include <map>
//#include <vector>
//#include <string>

//For sleep, finnes ingen <cunistd>
#include <unistd.h>
#include <iostream>


const int N_BUTTONS = 3;
int main() {
    OrderList orders;
    //Elevatormap init
    ElevatorMap elevators;
    //FSM init
    ElevatorFSM fsm = ElevatorFSM(&orders, &elevators);
    communication kom = communication(fsm);
    elevators.addElevator(kom.getIP(), 0);

    while(true) {
        kom.checkMailbox();
        static int prevSensor;
        int f = elev_get_floor_sensor_signal();
        if(f != -1){
            fsm.sensorActivated(f);
            if(f != prevSensor) {
                kom.sendMail(CURRENT_LOCATION, std::to_string(f));
                std::cout << "Sending location: " <<f << std::endl;//Fiks slik at dette skjer i starten også
            }
        }
        prevSensor = f;
        static int prev[N_FLOORS][N_BUTTONS];
        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                if(b==1 && f==0) continue; //Hindrer sjekking av ned i nedre etasje
                if(b==0 && f==N_FLOORS-1) continue; //Hindrer sjekking av opp i siste etasje
                int v = elev_get_button_signal((elev_button_type_t)b, f);
                if(v  &&  v != prev[f][b]){
                    //if(!orders.exists((elev_button_type_t)b, f)) {
                        fsm.buttonPressed((elev_button_type_t)b, f);
                        if(b==2) continue; //Hindrer sending av yttre knapper
                        kom.sendMail((elev_button_type_t)b, f);
                    //}
                }
                prev[f][b] = v;
            }
        }
        //DEBUG
        if(elev_get_stop_signal()){
            fsm.stopButtonPressed();
        }
        //END DEBUG
        usleep(100000);
    }
    return 0;
}

