#include "Backup.hpp"
Backup::Backup(std::string filename, ElevatorFSM *fsm_p) {
	backupFile = "";
	backupFile = filename;
	fsm = fsm_p;
}
void Backup::writeStringToFile(std::string str) {
    try {
        std::ofstream outFile(backupFile);
        outFile << str;
        outFile.close();
    } catch(...) {
        std::cerr << "Could not write to backupfile: " << backupFile << std::endl;
    }
}
std::string Backup::readStringFromFile() {
    std::string str;
    try {
        std::ifstream innFile(backupFile);
        innFile.seekg(0, std::ios::end);
        str.reserve(innFile.tellg());
        innFile.seekg(0, std::ios::beg);
        str.assign((std::istreambuf_iterator<char>(innFile)), std::istreambuf_iterator<char>());
        innFile.close();
    } catch(...) {
        std::cerr << "Could not read backupfile: " << backupFile << std::endl;
    }
    return str;
}
void Backup::restore(OrderList *orders) {
    std::string str = readStringFromFile();
    std::size_t first = str.find('{');
    std::size_t last = str.find('}');
    while((first != std::string::npos) || (last != std::string::npos) || (last < first)){
        std::string json = str.substr (first,last-first+1);
        str.erase(first,last-first+1);
        decodeJSON(json);
       	first = str.find('{');
        last = str.find('}');
    }
}
void Backup::make(OrderList *orders) {
	std::string output;
    for(int floor = 0; floor < N_FLOORS; floor++){
        if(orders->checkOrder(BUTTON_COMMAND, floor)) {
            output += toJSON(BUTTON_COMMAND, floor);
        }
    }
    writeStringToFile(output);
}
std::string Backup::toJSON(elev_button_type_t type, int floor){
    ptree pt;
    pt.put("type", type);
    pt.put("floor", floor);
    std::ostringstream buf;
    write_json(buf, pt);
    std::string json = buf.str();
    return json;
}
void Backup::decodeJSON(std::string json){
    std::size_t first = json.find("{");
    std::size_t last = json.find("}");
    if((first != std::string::npos) && (last != std::string::npos) && (last > first)) {
        ptree pt;
        json = json.substr (first,last-first+1);
        std::istringstream is(json);
        read_json(is, pt);
        fsm->buttonPressed((elev_button_type_t)pt.get<int>("type"), pt.get<int>("floor"));
    }
}