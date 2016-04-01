#pragma once
#include <ostream>
typedef enum tag_elev_lamp_type {
    BUTTON_CALL_UP = 0,
    BUTTON_CALL_DOWN = 1,
    BUTTON_COMMAND = 2
} elev_button_type_t;
class Button {
    private:
        elev_button_type_t type;
        int floor;
    public:
        Button(elev_button_type_t t, int f);
        int getFloor();
        elev_button_type_t getType();
        friend std::ostream &operator<<( std::ostream &output, const Button &Button_ref );
};
