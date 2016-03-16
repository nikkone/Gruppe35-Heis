#ifndef __INCLUDE_BUTTONLIST_HPP__
#define __INCLUDE_BUTTONLIST_HPP__
#include <ctime>
#include <ostream>
class ButtonList {//Funker like bra med time_t som med bool, men false=0 og true = time_t
    private:
        int tableSize;
        bool *table;//if time_t, 0 = not pressed, !0=time of buttonpress
    public:
        ButtonList(int floors);
        ~ButtonList();
        void setButton(int floor);
        bool getButton(int floor);
        void resetButton(int floor);
        int getSize();
        friend std::ostream &operator<<( std::ostream &output, const ButtonList &ButtonList_ref );
};
#endif //#ifndef __INCLUDE_BUTTONLIST_HPP__
