#include "communication.hpp"

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <ostream>
#include <vector>

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
communication::communication(ElevatorFSM &inputFsm) {
    ip = findmyip();
    com = new network(8001, ip);
    fsm = &inputFsm;
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
    //Strip empty space at end of json
    std::cout << "\""<< json <<"\""<<std::endl<< json.length() << std::endl;
    std::size_t found = json.find_last_of("}\\");
    json = json.substr(0,found) + "}";
    /*found = json.find_first_of("{\\");
    json = "{" + json.substr(0,found);*/

    std::cout << "\""<< json <<"\""<<std::endl<< json.length() << std::endl;
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
            std::cout << "Ip:" << ip << "Arrived at: " << floor << std::endl;
            break;
            /*
        case "SENDMEALL":
            break;
        case "CURRENT_FLOOR":
            break;
            */
    }
}
void communication::checkMailbox() {
    std::vector<std::string> mail = com->get_messages();
    for(std::vector<std::string>::iterator it = mail.begin(); it != mail.end(); it++) {
        //std::cout << *it << std::endl;
        decodeJSON(*it);
        
    }
}
void communication::sendMail(message_t type, std::string content) {
    com->send(toJSON(type, content));
}
void communication::sendMail(elev_button_type_t buttonType, int floor) {
    switch(buttonType) {
        case BUTTON_CALL_UP:
            sendMail(CALL_UP, std::to_string(floor));
            break;
        case BUTTON_CALL_DOWN:
            sendMail(CALL_DOWN, std::to_string(floor));
            break;
        case BUTTON_COMMAND:
            sendMail(COMMAND, std::to_string(floor));
            break;
    }
}