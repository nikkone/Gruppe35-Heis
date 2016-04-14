#include "communication.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <ostream>
#include <utility> 

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;


char* communication::findmyip() {
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
                                return p;
                            }
                        }
                   }
              }
        }
    return p;
}
address_v4 communication::getMyIP() {
    return myIP;
}
communication::communication(ElevatorMap *elevators_p) {
    myIP = address_v4::from_string(findmyip());
    com = new network(8001, myIP);
    elevators = elevators_p;
}

communication::~communication() {
    delete com;
}

std::string communication::toJSON(message_t type, int floor){
    ptree pt;
    pt.put("ip", myIP);
    pt.put("type", type);
    pt.put("floor", floor);//Trenger kanskje toString eller noe sånt?

    std::ostringstream buf;
    write_json(buf, pt);
    std::string json = buf.str();//kan kanskje returneres direkte?
    return json;
}

std::tuple<address_v4, message_t, int> communication::decodeJSON(std::string json){
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

std::vector<std::tuple<address_v4, message_t, int>> communication::checkMailbox() {
    std::vector<std::pair<address_v4, std::string >> mail = com->get_messages();
    std::vector<std::tuple<address_v4, message_t, int>> decodedMessages;
    for(std::vector<std::pair<address_v4, std::string >>::iterator it = mail.begin(); it != mail.end(); it++) {
        decodedMessages.push_back(decodeJSON(it->second));   
    }
    return decodedMessages;
}
void communication::updateElevatorMap() {
    std::vector<std::pair<address_v4, bool>> peers = com->get_listofPeers();
    for(std::vector<std::pair<address_v4, bool>>::iterator it = peers.begin(); it != peers.end(); it++) {
        std::cout << it->first << "->" << it->second << std::endl;
        if(it->second) {
            elevators->addElevator(it->first);
            sendMail(SENDMEALL, 0);
        } else {
            elevators->removeElevator(it->first);

        }
    }
}
void communication::sendMail(message_t type, int floor) {
    com->send(toJSON(type, floor));
}
void communication::sendMail(elev_button_type_t buttonType, int floor) {
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