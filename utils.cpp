#include "string.h"
#include "utils.h"
#include "zmq.h"

#include <assert.h>
#include <stdlib.h>
#include <thread>
#include <atomic>
#include <iostream>

#define SERVER_ADDR "tcp://127.0.0.1:8083"

void* context;
void* publisher;
zmq_msg_t reply;

void ffc::zmqInit() {
	context = zmq_ctx_new();
	publisher = zmq_socket(context, ZMQ_PUB);
	zmq_bind(publisher, SERVER_ADDR);
}

void ffc::zmqSendOrders(FfcOrder* master_orders, int ordersCount, int update, bool flag) {
	int v1, v2, v3; zmq_version(&v1, &v2, &v3);
	std::wcout << "ZMQ version " << v1 << "."  << v2 << "."  << v3 << "." << "Compile " << __DATE__ << __TIME__ << "\r\n";
	std::wcout << "ZMQ size " << ordersCount << "\r\n";

	int size = sizeof(int) * 2 + sizeof(FfcOrder)*ordersCount;
	zmq_msg_init_size(&reply, size);
	printf("size struct FfcOrder = %d \r\n", size);
	memcpy((int*)zmq_msg_data(&reply), &flag, sizeof(int));
	memcpy((int*)zmq_msg_data(&reply)+1, &update, sizeof(int));
	memcpy((int*)zmq_msg_data(&reply)+2, master_orders, sizeof(FfcOrder)*ordersCount);
	zmq_msg_send(&reply, publisher, 0);
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

}

void ffc::zmqDeInit() {
	zmq_msg_close(&reply);
	zmq_close(publisher);
	zmq_ctx_destroy(context);
}

int ffc::getMagic(wchar_t* comment) {
	wchar_t* pwc;
	pwc = wcstok(comment, L"_");
	pwc = wcstok(NULL, L"_");
	pwc = wcstok(NULL, L"_");
	if (pwc == NULL) return 0;
	return _wtoi(pwc);
}


int ffc::getComparison(FfcOrder* master_orders, int ticket, double tp, double sl, int count) {
	if (master_orders[count].ticket != ticket ||
		master_orders[count].tpprice != tp ||
		master_orders[count].slprice != sl) {
		return 1;
	}
	return 0;
}
