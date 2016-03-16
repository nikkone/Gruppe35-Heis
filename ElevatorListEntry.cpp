#include "ElevatorListEntry.hpp"
ElevatorListEntry::ElevatorListEntry(int floors) {
    elevatorButtons_p = new ButtonList(floors);
    upButtons_p = new ButtonList(floors);
    downButtons_p = new ButtonList(floors);
    //DISSE MAA FORANDRES PAA
    previousFloorVisited=0;
    Destination=0;
}
ElevatorListEntry::~ElevatorListEntry() {
    //delete elevatorButtonsPointer;
}
std::ostream &operator<<( std::ostream &output, const ElevatorListEntry &ElevatorListEntry_ref ) {
    output << "Knapper i heisen:" << std::endl << *ElevatorListEntry_ref.elevatorButtons_p << std::endl;
    output << "Oppknapper:" << std::endl << *ElevatorListEntry_ref.upButtons_p << std::endl;
    output << "Nedknapper:" << std::endl << *ElevatorListEntry_ref.downButtons_p << std::endl;
    output << "Sist besøkte etasje: " << ElevatorListEntry_ref.previousFloorVisited << std::endl;
    output << "Måletasje: " << ElevatorListEntry_ref.Destination << std::endl;
    return output;
}
void ElevatorListEntry::setFloor(int floor, elev_button_type_t button_type) {
    switch(button_type){
        default:
        case BUTTON_COMMAND:
            elevatorButtons_p->setButton(floor);
            break;
        case BUTTON_CALL_UP:
            upButtons_p->setButton(floor);
            break;
        case BUTTON_CALL_DOWN:
            downButtons_p->setButton(floor);
            break;
    }
}
void ElevatorListEntry::resetElevator(){
    int floors=elevatorButtons_p->getSize();
    delete elevatorButtons_p;
    elevatorButtons_p = new ButtonList(floors);
}
bool ElevatorListEntry::checkFloor(int floor, elev_motor_direction_t direction) {
    switch(direction){
        default:
        case DIRN_STOP:
            break;
        case DIRN_UP:
            if(upButtons_p->getButton(floor)) {
                return true;
            }
            break;
        case DIRN_DOWN:
            if(downButtons_p->getButton(floor)) {
                return true;
            }
            break;
    }
    if(elevatorButtons_p->getButton(floor)) {
        return true;
    }
    return false;
}
void ElevatorListEntry::resetFloor(int floor, elev_motor_direction_t direction) {
    switch(direction){
        default:
        case DIRN_STOP:
            break;
        case DIRN_UP:
            upButtons_p->resetButton(floor);
            break;
        case DIRN_DOWN:
            downButtons_p->resetButton(floor);
            break;
    }
    elevatorButtons_p->resetButton(floor);
}
/* TODO
CHECK-enum eller typedef for heisknapper
CHECK-enum eller typedef for heisrettning
Fikse socket
Fikse destructor
Fikse/ferdigstille initialisering
    FJERN at den hver gang lager nye ButtonList
    Ta inn pekere til disse listene som parameter
Funksjoner for plassering og destinasjon

*/
