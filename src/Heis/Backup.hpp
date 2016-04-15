#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "elev.h"
#include "OrderList.hpp"

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;
class Backup {
	private:
		std::string backupFile;
		void writeStringToFile(const std::string &str);
		std::string readStringFromFile();
		std::tuple<elev_button_type_t, int> readJSON(const std::string &json);
		std::string makeJSON(elev_button_type_t type, int floor);

	public:
		Backup(std::string filename);
		std::vector<std::tuple<elev_button_type_t, int>> readBackup();
		void writeBackup(OrderList *orders);
};