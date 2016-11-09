#pragma once

#include "ffcTypes.h"

namespace ffc {
	void zmqSendOrders(FfcOrder* master_orders, int ordersCount, int update, bool flag);
	int getMagic(wchar_t* comment);
	void zmqInit();
	void zmqDeInit();
	int getComparison(FfcOrder* master_orders, int ticket, double tp, double sl, int count);
}