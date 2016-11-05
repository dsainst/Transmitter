#include "windows.h"
#include <ctime>
#include <iostream>
#include <thread>
#include "utils.h"

#define MAX_ORDER_COUNT	200

//---------- Секция общей памяти -----------------
#pragma data_seg ("shared")
FfcOrder master_orders[MAX_ORDER_COUNT] = { 0 };
int ordersCount = 0;
int ordersTotal = 0;
bool ordersValid = false;
#pragma data_seg()
#pragma comment(linker, "/SECTION:shared,RWS")
//------------------------------------------------


//---------- Переменные трансмиттера -------------
bool transmitterInit = false;

namespace ffc {

	bool ffc_Init() {
		if (transmitterInit) return false;  //Повторная инициализация
		transmitterInit = true;
		ordersCount = 0;
		ordersValid = false;
		if (AllocConsole()) {
			freopen("CONOUT$", "w", stdout);
			freopen("conout$", "w", stderr);
			SetConsoleOutputCP(CP_UTF8);// GetACP());
			SetConsoleCP(CP_UTF8);
			std::wcout << "DLL inited.\r\n";
		}
		return true;  //Удачная инициализация
	}

	void ffc_OrderUpdate(int OrderTicket, int magic, wchar_t* OrderSymbol, int orderType,
		double OrderLots, double OrderOpenPrice, __time64_t OrderOpenTime,
		double OrderTakeProfit, double OrderStopLoss, double  OrderClosePrice, __time64_t  OrderCloseTime,
		__time64_t OrderExpiration, double  OrderProfit, double  OrderCommission, double  OrderSwap, wchar_t* OrderComment) {


		master_orders[ordersCount] = { OrderTicket, getMagic(OrderComment) , L"default", orderType, OrderLots, OrderOpenPrice, OrderOpenTime, OrderTakeProfit, OrderStopLoss, OrderClosePrice, OrderCloseTime, OrderExpiration, OrderProfit, OrderCommission, OrderSwap, L"" };

		wcscpy_s(master_orders[ordersCount].symbol, SYMBOL_LENGTH, OrderSymbol);

		wchar_t s2[20];
		_itow(OrderTicket, s2, 10);
		wcscpy_s(master_orders[ordersCount].comment, COMMENT_LENGTH, L"ffc_");
		wcscat(master_orders[ordersCount].comment, s2);

		std::wcout << "order #" << OrderTicket << " magic=" << master_orders[ordersCount].magic << " comment = " << master_orders[ordersCount].comment << "\r\n";
		ordersCount++;

	}

	int ffc_OrdersTotal() {
		std::wcout << "ffc_OrdersTotal: " << ordersTotal << "\r\n";
		return ordersTotal;
	}

	int ffc_GetOrderInfo() {
		MqlOrder mqlGetOrder[1] = { master_orders[ordersCount].ticket, master_orders[ordersCount].magic };
		return 1;
	}

	void ffc_ordersCount(int num) {
		ordersCount = 0;
		ordersTotal = num;
		std::wcout << "ordersTotal - " << ordersTotal << "\r\n";
	}

	void ffc_validation(bool flag) {
		ordersValid = flag;
		zmqSendOrders(master_orders);
		std::wcout << "Orders validation: " << ordersValid << "\r\n";
	}

	int ffc_GetTicket() { // выдаем тикет(ticket_id), о котором нам хотелось бы узнать
		return 0;
	}

	int ffc_OrderSelectError(int Ticket) {
		return Ticket;
	}

	int ffc_EndSession() {
		return true;
	}

	void ffc_DeInit() {
		transmitterInit = false;
	}


}