
#include "elev.h"
#include "OrderList.hpp"
#include "fsm.hpp"
#include "communication.hpp"
#include "ElevatorMap.hpp"
#include <unistd.h>
#include <iostream>

////////////////////////////      TODO       ///////////////////////////////
/*
    - LEGGE ALLE ENDRINGER UT PÅ NETTET OG LEGGE DEM INN LOKALT
        - Destinasjon
        + Lokasjon ved initialisering
        - Informasjon til nytilkoblet heis
    - Kostfunksjon for flere heiser
        + Stop på veien til destinasjon kun dersom ingen andre heiser er på vei til etasjen
        - Finn neste etasje med hensyn til andre heiser
            - Sjekk om jeg er nærmest av de ledige heisene, om det er uavgjort, avgjør på IP
    - Lagre backup til fil for å håndtere at datamaskinen mister strøm
    - Håndtering av at heisen mister strøm

*/
///////////////////////////////////////////////////////////////////////////

const int N_BUTTONS = 3;
int main() {
    OrderList orders;
    //Elevatormap init
    ElevatorMap elevators;
    //FSM init
    ElevatorFSM fsm = ElevatorFSM(&orders, &elevators);
    communication kom = communication(fsm);
    elevators.addElevator(kom.getIP(), 0);
    int prevSensor = -1;
    while(true) {
        kom.checkMailbox();
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

