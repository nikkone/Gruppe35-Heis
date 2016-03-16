#ifndef __INCLUDE_ORDER_HANDLING_H__
#define __INCLUDE_ORDER_HANDLING_H__

#include "elev.h"

#define N_FLOORS 4
#define N_BUTTONS 3

void oh_setOrder(int floor, elev_button_type_t buttonType);
void oh_resetFloorOrders(int floor);
void oh_resetAllOrders(void);
int oh_getNextOrder(int currentFloor, elev_motor_direction_t direction);
int oh_isFloorOrdered(int floor, elev_motor_direction_t direction, int nextFloor);

#endif //#ifndef __INCLUDE_ORDER_HANDLING_H__
