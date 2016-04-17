#pragma once
#include <tuple>
#include <vector>
#include <string>

#include "elev.h"
#include "OrderList.hpp"

class Backup {
	private:
		std::string backupFile;
		void writeStringToFile(const std::string &str);
		std::string readStringFromFile();
		std::tuple<elev_button_type_t, int> readJSON(const std::string &json);
		std::string makeJSON(elev_button_type_t type, int floor);

	public:
		Backup(const std::string &filename) : backupFile(filename) {};
		std::vector<std::tuple<elev_button_type_t, int>> readBackup();
		void writeBackup(OrderList &orders);
};