#include "Elevator.hpp"
Elevator::Elevator() {
    destination = -1;
    currentLocation = 0;
}
Elevator::Elevator(int dest, int loc) {
    destination = dest;
    currentLocation = loc;
}
std::ostream &operator<<( std::ostream &output, const Elevator &Elevator_ref ) {
    output << Elevator_ref.destination << "-" << Elevator_ref.currentLocation << std::endl;
    return output;
}
