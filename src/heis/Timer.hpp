#pragma once
#include <ctime>

class Timer {
    private:
        std::time_t endTime;
    public:
        Timer() : endTime(0) {};
        void set(int seconds);
        void reset();
        int check();
};
