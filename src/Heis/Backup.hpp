#pragma once
#include <string>
#include <iostream>
#include "fsm.hpp"
#include "elev.h"
#include <fstream>
#include <streambuf>
#include "OrderList.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;
class Backup {
	private:
		std::string backupFile;
		void writeStringToFile(std::string str);
		std::string readStringFromFile();
		void decodeJSON(std::string json);
		std::string toJSON(elev_button_type_t type, int floor);
		ElevatorFSM *fsm;
	public:
		Backup(std::string filename, ElevatorFSM *fsm_p);
		void restore(OrderList *orders);
		void make(OrderList *orders);
};