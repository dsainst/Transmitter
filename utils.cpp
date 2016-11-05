#include "string.h"
#include "utils.h"
#include "zmq.h"

#include <assert.h>
#include <stdlib.h>
#include <thread>
#include <atomic>
#include <iostream>

#define SERVER_ADDR "tcp://127.0.0.1:12345"

void ffc::zmqSendOrders(FfcOrder* master_orders) {
	int v1, v2, v3; zmq_version(&v1, &v2, &v3);
	std::wcout << "ZMQ version " << v1 << v2 << v3 << "Compile %s %s\n" << __DATE__ << __TIME__ << "\r\n";

	void* context = zmq_ctx_new();
	void* publisher = zmq_socket(context, ZMQ_PUB);

	zmq_bind(publisher, SERVER_ADDR);
	printf("Starting_v1\r\n");

	int counter = 0;

	for (;;)
	{
		printf("counter= %d \r\n", counter);
		counter++;
		zmq_msg_t reply;
		int size = sizeof(master_orders);
		zmq_msg_init_size(&reply, size);
		printf("size struct FfcOrder = %d", sizeof(master_orders));
		memcpy((int*)zmq_msg_data(&reply), master_orders, sizeof(master_orders));
		zmq_msg_send(&reply, publisher, 0);
		zmq_msg_close(&reply);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	}
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


