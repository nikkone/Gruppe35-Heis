#include "communication.hpp"

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <ostream>
#include <vector>
#include <map>

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
std::string communication::getIP() {
    return ip;
}
communication::communication(ElevatorFSM *inputFsm, ElevatorMap *elevators_p, OrderList *orders_p) {
    ip = findmyip();
    com = new network(8001, ip);
    fsm = inputFsm;
    elevators = elevators_p;
    orders = orders_p;
}

std::string communication::toJSON(message_t type, std::string content){
    ptree pt;
    pt.put("ip", ip);
    pt.put("type", type);
    pt.put("content", content);

    std::ostringstream buf;
    write_json(buf, pt);
    std::string json = buf.str();
    return json;
}

void communication::decodeJSON(std::string json){
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

        std::string ip = pt.get<std::string>("ip");
        message_t type = (message_t)pt.get<int>("type");
        int floor = pt.get<int>("content");
        switch(type) {
            case COMMAND:
                std::cout << "COMMAND" << floor << std::endl;
                fsm->buttonPressed(BUTTON_COMMAND, floor);
                break;
            case CALL_UP:
                fsm->buttonPressed(BUTTON_CALL_UP, floor);
                break;
            case CALL_DOWN:
                fsm->buttonPressed(BUTTON_CALL_DOWN, floor);
                break;  
            case CURRENT_LOCATION:
                elevators->setCurrentLocation(ip, floor);
                if( (floor == elevators->getDestination(ip)) && (floor != -1) ) {

                    if(floor!=0) {
                        elev_set_button_lamp(BUTTON_CALL_DOWN, floor, OFF);
                        orders->remove(BUTTON_CALL_DOWN, floor);
                    }

                    if(floor!=N_FLOORS-1) {
                        elev_set_button_lamp(BUTTON_CALL_UP, floor, OFF);
                        orders->remove(BUTTON_CALL_UP, floor);
                    }
                }
                //FOR DEBUG
                std::cout << "Ip:" << ip << "Arrived at: " << floor << std::endl;
                break;
            case DESTINATION:
            //SE GJWENNOM
                int currentDest = elevators->getDestination(ip);//FIX
                if(currentDest != -1 && floor == -1) {
                    if(currentDest!=0) {
                        elev_set_button_lamp(BUTTON_CALL_DOWN, currentDest, OFF);
                        orders->remove(BUTTON_CALL_DOWN, currentDest);
                    }

                    if(currentDest!=N_FLOORS-1) {
                        elev_set_button_lamp(BUTTON_CALL_UP, currentDest, OFF);
                        orders->remove(BUTTON_CALL_UP, currentDest);
                    }
                }
                elevators->setDestination(ip, floor);
                //FOR DEBUG
                std::cout << "Ip:" << ip << "Going to: " << floor << std::endl;
                break;
                
            case SENDMEALL:
                sendMail(CURRENT_LOCATION, elevators->getCurrentLocation());
                sendMail(DESTINATION, elevators->getDestination());
                for(int floor = 0; floor < N_FLOORS; floor++){
                    for(int button = 0; button < N_BUTTONS-1; button++){
                        if(button==1 && floor==0) continue; //Hindrer sjekking av ned i nedre etasje
                        if(button==0 && floor==N_FLOORS-1) continue; //Hindrer sjekking av opp i siste etasj
                        if(orders->checkOrder((elev_button_type_t)button, floor)) {
                            sendMail((elev_button_type_t)button, floor);
                        }
                    }
                }
                break;
        }
    }
}
void communication::checkMailbox() {
    std::vector<std::string> mail = com->get_messages();
    for(std::vector<std::string>::iterator it = mail.begin(); it != mail.end(); it++) {
        //std::cout << *it << std::endl;
        decodeJSON(*it);
        
    }

    std::map<std::string, bool> peers = com->get_listofPeers();
    for(std::map<std::string, bool>::iterator it = peers.begin(); it != peers.end(); it++) {
        std::cout << it->first << std::endl;
        if(it->second) {
            //elevators->addElevator(it->first);
            sendMail(SENDMEALL, 0);
        } else {
            elevators->removeElevator(it->first);
        }
    }

}
void communication::sendMail(message_t type, int content) {
    std::cout << content << std::endl;
    com->send(toJSON(type, std::to_string(content)));
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