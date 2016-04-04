#include <string>
#include <iostream>

#include <fstream>
#include <streambuf>
//Json
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;
typedef enum msg_types {
    CALL_UP, CALL_DOWN, COMMAND, CURRENT_LOCATION, DESTINATION
} message_t;
std::string toJSON(message_t type, std::string content) {
    std::string ip="192.168.1.178";    //DEBUG QUICKIE
    ptree pt;
    pt.put("ip", ip);
    pt.put("type", type);
    pt.put("content", content);

    std::ostringstream buf;
    write_json(buf, pt);
    std::string json = buf.str();
    return json;
}
void writeStringToFile(std::string str) {
    std::string input;
    std::ofstream out("file.txt");
    out << str;
    out.close();
}
std::string readStringFromFile() {
    std::ifstream t("file.txt");
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return str;
}
int main() {
    std::string str = readStringFromFile();
    std::size_t first, last;
    //unsigned int pos = 0;
    while(true) {
        //first = str.find('{', pos);
        //last = str.find('}', pos);
        first = str.find('{');
        last = str.find('}');
        //std::cout << str <<std::endl;
        if (first == std::string::npos) {
            //std::cout << "{ not found!" <<std::endl;
            break;
        } else if (last == std::string::npos) {
            //std::cout << "} not found!" <<std::endl;
            break;
        } else if (last < first) {
            //std::cout << "} before {!" <<std::endl;
            break;
        }/* else if(last+1 > (unsigned int)str.length()) {
            break;
        }*/ else {
            //std::cout << first << " : " << last  << " : " << str.length() << std::endl;
            std::string json = str.substr (first,last-first+1);
            str.erase(first,last-first+1);
            //pos = last+1;
            std::cout << "\"" << json << "\"" << std::endl;
        }
    }

/*
    std::string str = "dszfdasdfa" + toJSON(COMMAND, "1") + "sdfsdffgfd";
    std::size_t first = str.find("{");
    std::size_t last = str.find("}");
    if (first == std::string::npos) {
        std::cout << "{ not found!" << '\n';
    } else if (last == std::string::npos) {
        std::cout << "} not found!" << '\n';
    } else if (last < first) {
        std::cout << "} before {!" << '\n';
    } else {
        std::cout << first << " : " << last  << " : " << str.length() << std::endl;
        std::string strNew = str.substr (first,last-first+1);
        std::cout << "\"" << strNew << "\"" << std::endl;
    }
*/




    return 0;
}
