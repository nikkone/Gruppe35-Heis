#include "Button.hpp"

int Button::getFloor() const {
    return buttonFloor;
}

elev_button_type_t Button::getType() const {
    return buttonType;
}

std::ostream &operator<<( std::ostream &output, const Button &Button_ref ) {
    output << Button_ref.buttonType << "-" << Button_ref.buttonFloor << std::endl;
    return output;
}
