#include "Elevator.hpp"

std::ostream &operator<<( std::ostream &output, const Elevator &Elevator_ref ) {
    output << Elevator_ref.destination << "-" << Elevator_ref.currentLocation << std::endl;
    return output;
}
