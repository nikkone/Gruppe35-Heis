#pragma once

#include "elev.h"
#include "OrderList.hpp"
class ElevatorFSM {
    private:
    	OrderList *orders;
    public:
    	ElevatorFSM(OrderList* elevator);
		void buttonPressed(elev_button_type_t buttonType, int floor);
		void stopButtonPressed(void);
		void sensorActivated(int floor);
};