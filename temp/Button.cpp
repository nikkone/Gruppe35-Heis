#include "Button.hpp"
Button::Button(elev_button_type_t t, int f) {
    type=t;
    floor=f;
}
int Button::getFloor() {
    return floor;
}
elev_button_type_t Button::getType() {
    return type;
}
std::ostream &operator<<( std::ostream &output, const Button &Button_ref ) {
    output << Button_ref.type << "-" << Button_ref.floor << std::endl;
    return output;
}
