#include "Backup.hpp"
#include <fstream>
#include <iostream>
#include <streambuf>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

Backup::Backup(const std::string &filename) {
	backupFile = filename;
}

void Backup::writeStringToFile(const std::string &str) {
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

void Backup::writeBackup(OrderList &orders) {
	std::string output;
    for(int floor = 0; floor < N_FLOORS; ++floor){
        if(orders.checkOrder(BUTTON_COMMAND, floor)) {
            output += makeJSON(BUTTON_COMMAND, floor);
        }
    }
    writeStringToFile(output);
}

std::string Backup::makeJSON(elev_button_type_t type, int floor){
    ptree pt;
    pt.put("type", type);
    pt.put("floor", floor);
    std::ostringstream buf;
    write_json(buf, pt);
    return buf.str();
}

std::tuple<elev_button_type_t, int> Backup::readJSON(const std::string &json){
    ptree pt;
    std::istringstream is(json);
    read_json(is, pt);
    return std::make_tuple((elev_button_type_t)pt.get<int>("type"), pt.get<int>("floor"));
}

std::vector<std::tuple<elev_button_type_t, int>> Backup::readBackup() {
    std::string str = readStringFromFile();
    std::size_t first = str.find('{');
    std::size_t last = str.find('}');
    std::vector<std::tuple<elev_button_type_t, int>> decodedMessages;
    while((first != std::string::npos) && (last != std::string::npos) && (last > first)){
        std::string json = str.substr (first,last-first+1);
        str.erase(first,last-first+1);
        decodedMessages.push_back(readJSON(json));
        first = str.find('{');
        last = str.find('}');
    }
    return decodedMessages;
}
