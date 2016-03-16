#include "communication.hpp"
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

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

communication::communication() {
    ip = findmyip();
    com = new network(8001, ip);

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
    std::istringstream is(json);
    read_json(is, pt);
    std::string ip = pt.get<std::string>("ip");
    message_t type = (message_t)pt.get<int>("type");
    switch(type) {
        case BUTTON_COMMAND:
            break;
        case BUTTON_CALL_UP:
            break;
        case BUTTON_CALL_DOWN:
            break;
            /*
        case "STATE":
            break;
        case "SENDMEALL":
            break;
        case "CURRENT_FLOOR":
            break;
            */
    };
}