#include "string.h"
#include "utils.h"
//#include "WinSock2.h"
#include "zmq.h"

#include <assert.h>
#include <stdlib.h>
#include <thread>
#include <atomic>
#include <iostream>
#include <iterator>

#define SERVER_ADDR "tcp://*:8083"
//#define SERVER_ADDR "tcp://192.168.32.15:8083"
//#define SERVER_ADDR "tcp://212.116.110.46:8083"


void* context;
void* publisher;
zmq_msg_t reply;
std::map<int, int>	ticketMap;
int lastNick = 0;

bool ffc::zmqInit() {
	context = zmq_ctx_new();
	if (!context) return false;
	publisher = zmq_socket(context, ZMQ_PUB);
	if (!publisher) return false;
	int res = zmq_bind(publisher, SERVER_ADDR);
	if (res != 0) return false;
	int v1, v2, v3; zmq_version(&v1, &v2, &v3);
	std::wcout << "ZMQ version " << v1 << "." << v2 << "." << v3 << "." << "Compile " << __DATE__ << __TIME__ << "\r\n";
	return true;
}

void ffc::zmqSendOrders(FfcMsg* msg) {
	if (!context || !publisher) {
		std::wcout << "Try reinit ZMQ...\r\n";
		zmqDeInit();
		if (!zmqInit()) return;
	}
	std::wcout << "ZMQ send " << msg->ordersCount << " orders\r\n"; 

	int size = sizeof(int) * 2 + sizeof(FfcOrder) * msg->ordersCount;
	if (zmq_msg_init_size(&reply, size) != 0) {
		std::wcout << "ZMQ allocate message error!\r\n";
		zmqDeInit();
		return;
	}
	printf("size struct FfcOrder = %d \r\n", size);
	memcpy(zmq_msg_data(&reply), msg, size);
	if (zmq_msg_send(&reply, publisher, 0) <= 0) {
		std::wcout << "ZMQ send message error!\r\n";
		zmqDeInit();
	}
}

void ffc::zmqDeInit() {
	zmq_msg_close(&reply);
	if (publisher) {
		zmq_close(publisher);
		publisher = nullptr;
	}
	if (context) {
		zmq_ctx_destroy(context);
		context = nullptr;
	}
}

int ffc::getProvider(wchar_t* comment) {
	wchar_t* pwc;
	pwc = wcstok(comment, L"_");
	pwc = wcstok(NULL, L"_");
	pwc = wcstok(NULL, L"_");
	if (pwc == NULL) return 0;
	return _wtoi(pwc);
}

int ffc::getMap(int ticket) {
	auto itr = ticketMap.find(ticket);
	if (itr != ticketMap.end()) {	//”же есть
		return itr->second;
	}
	lastNick++;
	if (lastNick >= 4096) lastNick = 1;
	ticketMap[ticket] = lastNick;
	
	return lastNick;
}

typedef std::pair<int, int> mypair;

void ffc::saveMap() {
	std::ofstream f("ticketmap.dat", std::ios::out | std::ios::binary);
	int mapSize = ticketMap.size();
	f.write((char*)&mapSize, sizeof(mapSize));
	f.write((char*)&lastNick, sizeof(lastNick));
	std::copy(ticketMap.begin(), ticketMap.end(), std::ostream_iterator<mypair>(f));
	f.close();
}

void ffc::loadMap() {
	std::ifstream f("ticketmap.dat", std::ios::in | std::ios::binary);
	int mapSize;
	f.read((char*)&mapSize, sizeof(mapSize));
	f.read((char*)&lastNick, sizeof(lastNick));
	std::copy(std::istream_iterator<mypair>(f), std::istream_iterator<mypair>(), std::inserter(ticketMap, ticketMap.begin()));
	f.close();
}

