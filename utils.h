#pragma once
#include "ffcTypes.h"

namespace ffc {
	void zmqSendOrders(FfcOrder* master_orders);
	int getMagic(wchar_t* comment);
}