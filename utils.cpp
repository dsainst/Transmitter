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
bool updated = false;
char workpath[255] = { 0 };

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
	//std::wcout << "ZMQ send ticket " << msg->orders->ticket << "\r\n";

	int size = sizeof(int) * 2 + sizeof(FfcOrder) * msg->ordersCount;
	if (zmq_msg_init_size(&reply, size) != 0) {
		std::wcout << "ZMQ allocate message error!\r\n";
		zmqDeInit();
		return;
	}
//	printf("size struct FfcOrder = %d \r\n", size);
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

int ffc::getTicket(wchar_t* comment) {
	wchar_t* pwc;
	pwc = wcstok(comment, L"#");
	pwc = wcstok(NULL, L"#");
	if (pwc == NULL) return 0;
	return _wtoi(pwc);
}

int ffc::getMap(int ticket, int new_ticket) {
	auto itr = ticketMap.find(ticket);
	if (itr != ticketMap.end()) {	//��� ����
		//std::wcout << "itr->second1 - " << itr->second << " ticket - " << ticket << "\r\n";
		if (new_ticket) {
			//std::wcout << "itr->second2 - " << itr->second << " ticket = " << ticket << " ticket new - " << new_ticket << " ticket map end - " << "\r\n";
			ticketMap[new_ticket]	= itr->second;
			ticketMap.erase(ticket);
			updated = true;
		}
		return itr->second;
	}
	if (new_ticket) return 0; // ���� �� ����� �� #from
	lastNick++;
	if (lastNick >= 4096) lastNick = 1;
	ticketMap[ticket] = lastNick;
	//std::wcout << " ticket - " << ticket << " ticketMap[ticket] - " << ticketMap[ticket] << "\r\n";
	updated = true;
	
	return lastNick;
}

void ffc::resetFlag() {
	updated = false;
}

void ffc::clearMap() {
	ticketMap.clear();
	lastNick = 0;
}

typedef std::pair<int, int> mypair;


void ffc::saveMap() {
	if (!updated) return;
	std::ofstream file(workpath, std::ios::out | std::ios::binary);
	if (!file) {
		std::wcout << "SaveMap open error! \r\n";
		return;
	}
	int mapSize = ticketMap.size();
	file.write((char*)&mapSize, sizeof(mapSize));
	file.write((char*)&lastNick, sizeof(lastNick));

	for (auto itr = ticketMap.begin(); itr != ticketMap.end(); itr++) {
		file.write((char*)&(itr->first), sizeof(int));
		file.write((char*)&(itr->second), sizeof(int));
	}
	if (file.bad()) {
		std::wcout << "SaveMap write file error! \r\n";
	}
	file.close();
}

void ffc::loadMap() {
	std::ifstream file(workpath, std::ios::out | std::ios::binary);
	if (file.fail()) { return; } 
	else {
		int mapSize = ticketMap.size();
		file.read((char*)&mapSize, sizeof(mapSize));
		file.read((char*)&lastNick, sizeof(lastNick));
		std::wcout << "mapSize: " << mapSize << " - lastNick: " << lastNick << "\r\n";
		while (1) {
			int key, value;
			file.read((char*)&key, sizeof(int));
			file.read((char*)&value, sizeof(int));
			if (file.eof()) return;
			ticketMap[key] = value;
			std::wcout << "Map: master ticket: " << key << " - " << value << "\r\n";
		}
	}
	file.close();
}


void ffc::setMapFile(wchar_t* line) {
	if (line == nullptr) return;
	WideCharToMultiByte(CP_ACP, 0, line, 255, &workpath[0], 255, NULL, NULL);
	strcat_s(workpath, "/MQL4/Experts/ticketmap.dat");
	std::wcout << "Base path: " << workpath << "\r\n";
}

