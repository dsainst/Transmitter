#pragma once
#include <map>
#include <iostream>
#include <fstream>
#include "ffcTypes.h"

namespace ffc {
	void zmqSendOrders(FfcMsg* msg);
	int getProvider(wchar_t* comment);
	int getTicket(wchar_t* comment);
	bool zmqInit();
	void zmqDeInit();
	int getMap(int ticket, int new_ticket);
	void clearMap();
	void saveMap();
	void loadMap();
	void resetFlag();
	bool getFlag();
	void setMapFile(wchar_t* line);
}