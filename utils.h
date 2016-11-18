#pragma once
#include <map>
#include <iostream>
#include <fstream>
#include "ffcTypes.h"

namespace ffc {
	void zmqSendOrders(FfcMsg* msg);
	int getProvider(wchar_t* comment);
	bool zmqInit();
	void zmqDeInit();
	int getMap(int ticket);
	void saveMap();
	void loadMap();
}