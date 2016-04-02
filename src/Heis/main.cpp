
#include "elev.h"
#include "OrderList.hpp"
#include "fsm.hpp"
#include "communication.hpp"

//for exit
//#include <cstdlib>
//For sleep, finnes ingen <cunistd>
//#include <map>
//#include <vector>
#include <unistd.h>

#include <string>


#include <iostream>


const int N_BUTTONS = 3;
int main() {
    OrderList orders;
    //FSM init
    ElevatorFSM fsm = ElevatorFSM(&orders);
    communication kom = communication(fsm);
    //int sensorSignal;
    while(true) {
        kom.checkMailbox();

            static int prevSensor;
            int f = elev_get_floor_sensor_signal();
            if(f != -1  &&  f != prevSensor){
                fsm.sensorActivated(f);
                kom.sendMail(CURRENT_LOCATION, std::to_string(f));
            }
            prevSensor = f;

            static int prev[N_FLOORS][N_BUTTONS];
            for(int f = 0; f < N_FLOORS; f++){
                for(int b = 0; b < N_BUTTONS; b++){

                    if(b==1 && f==0) continue;

                    if(b==0 && f==N_FLOORS-1) continue;
                    int v = elev_get_button_signal((elev_button_type_t)b/*BUTTON_COMMAND*/, f);
                    if(v  &&  v != prev[f][b]){
                        //if(!orders.exists((elev_button_type_t)b, f)) {
                            fsm.buttonPressed((elev_button_type_t)b, f);
                            if(b==2) continue; //Hindrer sending av Heispanelknapper
                            kom.sendMail((elev_button_type_t)b, f);
                        //}
                    }
                    prev[f][b] = v;
                }
        }
        if(elev_get_stop_signal()){
            fsm.stopButtonPressed();
        }
        usleep(100000);
    }
    return 0;
}

