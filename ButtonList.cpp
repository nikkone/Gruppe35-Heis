#include "ButtonList.hpp"
ButtonList::ButtonList(int floors) {
    tableSize = floors;
    table=new bool[floors];
    for(int i=0;i<floors;i++) {
        table[i]=false;
    }
}
ButtonList::~ButtonList() {
    delete[] table;
}
std::ostream &operator<<( std::ostream &output, const ButtonList &ButtonList_ref ) {
    for(int i=0; i<ButtonList_ref.tableSize;i++){
        output << ButtonList_ref.table[i] << " - ";
    }
    output << std::endl;
    return output;
}
void ButtonList::setButton(int floor) {
    table[floor]=true;
}
void ButtonList::resetButton(int floor) {
    table[floor]=false;
}
int ButtonList::getSize() {
    return tableSize;
}
bool ButtonList::getButton(int floor){
    return table[floor];
}
