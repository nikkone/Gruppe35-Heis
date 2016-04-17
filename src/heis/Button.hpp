#pragma once
#include <ostream>
#include "elev.h"

class Button {
    private:
        elev_button_type_t buttonType;
        int buttonFloor;
    public:
        Button(elev_button_type_t type, int floor) : buttonType(type), buttonFloor(floor) {};
        int getFloor() const;
        elev_button_type_t getType() const;
        friend std::ostream &operator<<( std::ostream &output, const Button &Button_ref );
};