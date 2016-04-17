#include <sstream>
#include <ostream>
#include <utility> 
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Communication.hpp"

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;


address_v4 Communication::findMyIP() {
    try{
        FILE * fp = popen("ifconfig", "r");
        char* p, *e;
        if (fp) {
            p=NULL; size_t n;
            while ((getline(&p, &n, fp) > 0) && p) {
                if ( (p = strstr(p, "inet ")) ) {
                    p+=5;
                    if ( (p = strchr(p, ':')) ) {
                        ++p;
                        if ( (e = strchr(p, ' ')) ) {
                            *e='\0';
                            pclose(fp);
                            return address_v4::from_string(p);
                        }
                    }
                }
            }
        }
    } catch(...) {
        std::cerr << "Could not find ip from ifconfig" << std::endl;
    }
    std::cout << "Write your IP now (or 0.0.0.0): ";
    std::string input;
    std::getline(std::cin, input);
    try {
        return address_v4::from_string(input);
    } catch(...) {
        std::cerr << "Could not parse ip, exiting!" << std::endl;
        exit(1);
    }
}

const address_v4 Communication::getMyIP() const{
    return network.getMyIP();
}

std::string Communication::makeJSON(message_t type, int floor){
    ptree pt;
    pt.put("ip", getMyIP());
    pt.put("type", type);
    pt.put("floor", floor);

    std::ostringstream buf;
    write_json(buf, pt);
    return buf.str();
}

std::tuple<address_v4, message_t, int> Communication::readJSON(std::string json){
    std::size_t first = json.find("{");
    std::size_t last = json.find("}");
    if((first != std::string::npos) && (last != std::string::npos) && (last > first)) {
        ptree pt;
        json = json.substr (first,last-first+1);
        std::istringstream is(json);
        read_json(is, pt);
        return std::make_tuple(address_v4::from_string(pt.get<std::string>("ip")), (message_t)pt.get<int>("type"), pt.get<int>("floor"));
    }
    return std::make_tuple(address_v4(), FAILED, -1);
}

const std::vector<std::tuple<address_v4, message_t, int>> Communication::checkMailbox() {
    std::vector<std::pair<address_v4, std::string >> mail = network.get_messages();
    std::vector<std::tuple<address_v4, message_t, int>> decodedMessages;
    for(std::vector<std::pair<address_v4, std::string >>::iterator it = mail.begin(); it != mail.end(); ++it) {
        decodedMessages.push_back(readJSON(it->second));   
    }
    return decodedMessages;
}

void Communication::updateElevatorMap(ElevatorMap &elevators) {
    std::vector<std::pair<address_v4, bool>> peers = network.get_listofPeers();
    for(std::vector<std::pair<address_v4, bool>>::const_iterator it = peers.begin(); it != peers.end(); ++it) {
        if(it->second) {
            elevators.addElevator(it->first);
            sendMail(SENDMEALL, 0);
        } else {
            elevators.removeElevator(it->first);

        }
    }
}

void Communication::sendMail(message_t type, int floor) {
    network.send(makeJSON(type, floor));
}

void Communication::sendMail(elev_button_type_t buttonType, int floor) {
    switch(buttonType) {
        case BUTTON_CALL_UP:
            sendMail(CALL_UP, floor);
            break;
        case BUTTON_CALL_DOWN:
            sendMail(CALL_DOWN, floor);
            break;
        case BUTTON_COMMAND:
            sendMail(COMMAND, floor);
            break;
    }
}