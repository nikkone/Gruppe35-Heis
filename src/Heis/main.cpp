#include <unistd.h>
#include <iostream>
#include <boost/asio/ip/address_v4.hpp>

#include "elev.h"
#include "OrderList.hpp"
#include "fsm.hpp"
#include "communication.hpp"
#include "ElevatorMap.hpp"
#include "Backup.hpp"
using boost::asio::ip::address_v4;
////////////////////////////      TODO       ///////////////////////////////
/*
    + LEGGE ALLE ENDRINGER UT PÅ NETTET OG LEGGE DEM INN LOKALT
        + Destinasjon
        + Lokasjon ved initialisering
        + Informasjon til nytilkoblet heis
        + Fikse at det noen ganger kommer to(like) heiser ved addElevator()
        + Fjerning av heiser fra listen dersom de kobles fra.
    + Kostfunksjon for flere heiser
        + Stop på veien til destinasjon kun dersom ingen andre heiser er på vei til etasjen
        + Finn neste etasje med hensyn til andre heiser
            + Sjekk om jeg er nærmest av de ledige heisene, om det er uavgjort, avgjør på IP
            + Ta inn ElevatorMap i getNextFloor() muligens?
    + Lagre backup til fil for å håndtere at datamaskinen mister strøm
        + Tenke over hvilke knapper som blir lagret til backup
        + Timer som kjører backup ved jevne mellomrom
        + Fikse lesing av fil hvis fil ikke finnes
    + Lage watchdog som gjenstarter programmet dersom det ikke responderer.
        + Drepe programmet om det er åpent(og da ikke responderer)
        + Restarte programmet
    ? Lage funksjon som oppdaterer lysene ut fra orderlist
    - Håndtering av at heisen(men ikke datamaskinen) mister strøm
    + Nettverksmodul
        + Se gjennom if og while-løkkene i receive()
        + Mutex rundt get rutinene?
        + Bedre navn på respond, sendToAll eller noe sånt?
    - Kommunikasjon
        + Dele opp decodeJSON()
        + Sette decodeJSON tilbake til å ta ip fra jSON
        + Sjekke om ip er mindre enn 3 tall
        - Dele opp checkMailboks?
        - Sendmeall kun til den første som heisen kobler seg til?
        - Svare på Sendmeall kun til den som spør? IKKE VIKTIG
    - OrderList
        + Skifte navn på exists til isButtonOrdered eller noe mer beskrivende
        - Fjerne first på en eller annen måte
    - ElevatorFSM
        + Endre navn på staten RUNNING til MOVING eller noe sånt(Tips fra Anders)
    - ElevatorMap
        - Endre navnet på funksjonene som bruker first til getMyDest osv.
        ? Endre navner på first kanskje
    - Gjøre klassene våre mer komplette
        + Destructors hvis dynamisk alokert 
    - Overalt
        + Endre IPer til adress_v4 i boost biblioteket OVERALT
        - Flytte includes som ikke er brukt i headeren over i cpp filene
        - Hive inn noen std::cerr der exeptions blir kalt.

*/
///////////////////////////////////////////////////////////////////////////


const int backupInterval = 1;//seconds
int main() {
    OrderList orders;
    ElevatorMap elevators;
    Timer* motorTimer = new Timer();
    ElevatorFSM fsm = ElevatorFSM(&orders, &elevators, motorTimer);
    communication kom = communication(&fsm, &elevators, &orders);
    elevators.addElevator(kom.getIP(), 0);
    Backup backup("backup.txt", &fsm);//skift til .json?
    backup.restore(&orders);
    Timer* backupTimer = new Timer();//Endre til ikke dynamisk alokert
    backupTimer->set(backupInterval);
    int previousFloorSensor = -1;
    int previousDestination= -1;
    //std::system("xterm -e \"./bin/watchdog\" &");
    while(true) {
        kom.checkMailbox();
        //Send destination
        if(previousDestination != elevators.getDestination()) {
            previousDestination = elevators.getDestination();
            std::cout << "Sending destination: " << previousDestination << std::endl;
            kom.sendMail(DESTINATION, previousDestination);
        }

        int floorSensorSignal = elev_get_floor_sensor_signal();
        if(floorSensorSignal != -1) {
            if(floorSensorSignal != previousFloorSensor) {
                kom.sendMail(CURRENT_LOCATION, floorSensorSignal);
                std::cout << "Sending location: " << floorSensorSignal << std::endl;//Fiks slik at dette skjer i starten også, tror fikset?
                previousFloorSensor = floorSensorSignal;//Sjekk legginn i if setning???????????    
            }
            fsm.floorSensorActivated(floorSensorSignal);
        }
        //FLYTT PÅ KANSJKE? FARLIG Å FLYTTE PÅ POASS PÅ
        if(orders.getNextFloor(&elevators) != -1) {
            fsm.newDestination(orders.getNextFloor(&elevators));
            //std::cout << "New order: "<< std::endl;
        }
        static bool prev[N_FLOORS][N_BUTTONS];
        for(int floor = 0; floor < N_FLOORS; floor++){
            for(int button = 0; button < N_BUTTONS; button++){
                if(button==BUTTON_CALL_DOWN && floor==0) continue; //Hindrer sjekking av ned i nedre etasje
                if(button==BUTTON_CALL_UP && floor==N_FLOORS-1) continue; //Hindrer sjekking av opp i siste etasje
                bool buttonSignal = elev_get_button_signal((elev_button_type_t)button, floor);
                if(buttonSignal  &&  buttonSignal != prev[floor][button]){
                    //if(!orders.checkOrder((elev_button_type_t)button, floor)) {
                        fsm.buttonPressed((elev_button_type_t)button, floor);
                        if(button!=BUTTON_COMMAND) {
                            kom.sendMail((elev_button_type_t)button, floor);
                        }
                    //}
                }
                prev[floor][button] = buttonSignal;//Sjekk legginn i if setning
            }
        }

        //DEBUG
        if(elev_get_stop_signal()){
            fsm.stopButtonPressed();
        }
        //END DEBUG
        if(backupTimer->check()) {
            backup.make(&orders);
            backupTimer->set(backupInterval);
        }
        if(motorTimer->check()) {
            return 1;
        }
        usleep(100000);
    }

    return 0;
}