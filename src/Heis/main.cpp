
#include "elev.h"
#include "OrderList.hpp"
#include "fsm.hpp"
#include "communication.hpp"
#include "ElevatorMap.hpp"
#include <unistd.h>
#include <iostream>
#include "Backup.hpp"
////////////////////////////      TODO       ///////////////////////////////
/*
    - LEGGE ALLE ENDRINGER UT PÅ NETTET OG LEGGE DEM INN LOKALT
        + Destinasjon
        + Lokasjon ved initialisering
        + Informasjon til nytilkoblet heis
        - Fikse at det noen ganger kommer to(like) heiser ved addElevator()
        - Fjerning av heiser fra listen dersom de kobles fra.
    - Kostfunksjon for flere heiser
        + Stop på veien til destinasjon kun dersom ingen andre heiser er på vei til etasjen
        - Finn neste etasje med hensyn til andre heiser
            - Sjekk om jeg er nærmest av de ledige heisene, om det er uavgjort, avgjør på IP
            ? Ta inn ElevatorMap i getNextFloor() muligens?
    - Lagre backup til fil for å håndtere at datamaskinen mister strøm
        * Mulig fix er at i Backup::make() så har ikke stringen blitt laget, bare deklarert?
        * Evt. Bare ta bort kommenteringen?
        - Tenke over hvilke knapper som blir lagret til backup
        - Timer som kjører backup ved jevne mellomrom
    - Lage watchdog som gjenstarter programmet dersom det ikke responderer.
        - Drepe programmet om det er åpent(og da ikke responderer)
        - Restarte programmet
    - Håndtering av at heisen(men ikke datamaskinen) mister strøm
    - Bedre timer
        - Muligens flertråds med sleep slik at det blir mer nøyaktig 3sec
    - Nettverksmodul
        - Se gjennom if og while-løkkene i receive()
    - OrderList
        - Skifte navn på exists til isButtonOrdered eller noe mer beskrivende
    - ElevatorFSM
        - Endre navn på staten RUNNING til MOVING eller noe sånt(Tips fra Anders)
    - ElevatorMap
        - Endre navnet på funksjonene som bruker first til getMyDest osv.
        ? Endre navner på first kanskje
    - Gjøre klassene våre mer komplette
        - Destructors hvis dynamisk alokert 

*/
///////////////////////////////////////////////////////////////////////////
int main() {
    OrderList orders;
    //Elevatormap init
    ElevatorMap elevators;
    //FSM init
    ElevatorFSM fsm = ElevatorFSM(&orders, &elevators);
    communication kom = communication(&fsm, &elevators, &orders);
    elevators.addElevator(kom.getIP(), 0);
    int prevSensor = -1;
    int tempDest= -1;
    //Les inn backup
    Backup backup("backup.txt", &fsm);//skift til .json?
    //backup.restore(&orders);
    while(true) {
        kom.checkMailbox();

        //Send destination
        if(tempDest != elevators.getDestination()) {
            tempDest = elevators.getDestination();
            kom.sendMail(DESTINATION, tempDest);
        }
        int f = elev_get_floor_sensor_signal();
        if(f != -1){
            fsm.sensorActivated(f);
            if(f != prevSensor) {
                kom.sendMail(CURRENT_LOCATION, f);
                std::cout << "Sending location: " <<f << std::endl;//Fiks slik at dette skjer i starten også
            }
        }
        prevSensor = f;//Sjekk legginn i if setning
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
                prev[f][b] = v;//Sjekk legginn i if setning
            }
        }
        //DEBUG
        if(elev_get_stop_signal()){
            fsm.stopButtonPressed();
        }
        //Skriv backup med mulig timer
        //backup.make(&orders);
        //END DEBUG
        usleep(100000);
    }

    return 0;
}

