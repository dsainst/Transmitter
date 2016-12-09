#include "windows.h"
#include <time.h>
#include <iostream>
#include <thread>
#include "utils.h"
#include <map>

#define TIMEOUT 60

FfcMsg	master;
int		ordersTotal		= 0;
time_t	lastSession = 0;
int		timeRestart		= 0;
bool	transmitterInit	= false;
bool	needUpdate		= false;
bool	noCycle			= true;


namespace ffc {

	bool ffc_Init(wchar_t* path) {
		if (transmitterInit) return false;  //Повторная инициализация
		setMapFile(path);
		transmitterInit = true;				//Защита от двойного запуска
		if (AllocConsole()) {
			freopen("CONOUT$", "w", stdout);
			freopen("conout$", "w", stderr);
			SetConsoleOutputCP(CP_UTF8);// GetACP());
			SetConsoleCP(CP_UTF8);
			std::wcout << "DLL inited.v3.\r\n";
		}
		if (!zmqInit()) {
			std::wcout << "ZMQ init error!\r\n";
			zmqDeInit();
			return false;
		}
		loadMap();
		master.ordersCount = 0;
		master.validation = false;
		std::wcout << "initDone \r\n";
		return true;  //Удачная инициализация

	}

	//Начало цикла обработки 
	void ffc_ordersCount(int num) {
		needUpdate = ordersTotal != num;   //Если предыдущее количество ордеров другое, то обновляем
		//std::wcout << "needUpdate: (ordersTotal != num) " << needUpdate << "\r\n"; 
		master.ordersCount = 0;
		ordersTotal = num;
		resetFlag();
		//std::wcout << "ordersTotal - " << ordersTotal << "\r\n";
	}

	void ffc_OrderUpdate(int ticket, int magic, wchar_t* symbol, int opType,
		double lots, double openPrice, double takeProfit, double stopLoss, 
		__time64_t expiration, wchar_t* comment) {
		int provider;
		int mapedTicket;

		FfcOrder* order = master.orders + master.ordersCount;

		//проверка на from#, если есть то подменяем тикет, ticket_from - старый, закрытый тикет
		int ticket_from = getTicket(comment);
		if (ticket_from) {
			provider = 1;
			mapedTicket = getMap(ticket_from, ticket);
		} else provider = getProvider(comment);
		if (!mapedTicket) mapedTicket = getMap(ticket, 0);

		needUpdate = needUpdate || order->ticket != mapedTicket || order->tpprice != takeProfit || order->slprice != stopLoss || order->lots != lots;
		//std::wcout << "lots master - " << lots << "\r\n";

		order->ticket		= mapedTicket;	//Надо сделать маппинг
		order->magic		= provider;
		order->type			= opType;
		order->lots			= lots;
		order->openprice	= openPrice;
		order->tpprice		= takeProfit;
		order->slprice		= stopLoss;
		order->expiration	= expiration;

		wcscpy_s(order->symbol, SYMBOL_LENGTH, symbol);

		master.ordersCount++;
	}

	//Конец цикла передачи открытых ордеров
	void ffc_validation(bool flag) {
		master.validation = flag;
		if (needUpdate || (time(NULL) - lastSession) > TIMEOUT) {
			zmqSendOrders(&master);
			lastSession = time(NULL);
		}
		saveMap();
		//std::wcout << "Orders validation: " << timeRestart << "\r\n";
		
	}

	int ffc_GetTicket() { // выдаем тикет(ticket_id), о котором нам хотелось бы узнать
		
		return 0;
	}

	void ffc_OrderSelectError(int Ticket) {
	}

	void ffc_DeInit() {
		zmqDeInit();
		transmitterInit = false;
	}


}