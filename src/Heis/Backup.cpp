#include "Backup.hpp"
Backup::Backup(std::string filename, ElevatorFSM *fsm_p) {
	backupFile = "";
	backupFile = filename;
	fsm = fsm_p;
}
void Backup::writeStringToFile(std::string str) {
    std::string input;
    std::ofstream out(backupFile);
    out << str;
    out.close();
}
std::string Backup::readStringFromFile() {
    std::ifstream t(backupFile);
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return str;
}
void Backup::restore(OrderList *orders) {
    std::string str = readStringFromFile();
    std::size_t first = str.find('{');
    std::size_t last = str.find('}');
    while((first != std::string::npos) || (last != std::string::npos) || (last < first)){
        //std::cout << str <<std::endl;
        std::string json = str.substr (first,last-first+1);
        str.erase(first,last-first+1);
        //std::cout << "\"" << json << "\"" << std::endl;
        decodeJSON(json);
       	first = str.find('{');
        last = str.find('}');
    }
}
void Backup::make(OrderList *orders) {
	std::string output;
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS-1; b++){
            if(b==1 && f==0) continue; //Hindrer sjekking av ned i nedre etasje
            if(b==0 && f==N_FLOORS-1) continue; //Hindrer sjekking av opp i siste etasj
            if(orders->exists((elev_button_type_t)b, f)) {
                output += toJSON((elev_button_type_t)b, f);
            }
        }
    }
    writeStringToFile(output);
}
std::string Backup::toJSON(elev_button_type_t type, int floor){
    ptree pt;
    pt.put("type", type);
    pt.put("content", floor);

    std::ostringstream buf;
    write_json(buf, pt);
    std::string json = buf.str();
    return json;
}
void Backup::decodeJSON(std::string json){
    ptree pt;
    std::size_t first = json.find("{");
    std::size_t last = json.find("}");
    if (first == std::string::npos) {
        std::cout << "{ not found!" << std::endl;
        std::cout << json << std::endl;
    } else if (last == std::string::npos) {
        std::cout << "} not found!" << std::endl;
    } else if (last < first) {
        std::cout << "} before {!" << std::endl;
    } else {
        json = json.substr (first,last-first+1);
        std::istringstream is(json);
        read_json(is, pt);
        elev_button_type_t type = (elev_button_type_t)pt.get<int>("type");
        int floor = pt.get<int>("content");
        fsm->buttonPressed(type, floor);
    }
}