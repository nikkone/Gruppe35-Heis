#pragma once
#include <ctime>
class timer {
    private:
        std::time_t endTime;
    public:
        timer();
        void set(int seconds);
        int check();
};
