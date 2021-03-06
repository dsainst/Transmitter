//+------------------------------------------------------------------+
//|                                                  Transmitter.mq4 |
//|                        Copyright 2016, BlackSteel, FairForex.org |
//|                                            https://fairforex.org |
//+------------------------------------------------------------------+
#property copyright "Copyright 2016, BlackSteel, FairForex.org"
#property link      "https://fairforex.org"
#property version   "1.00"
#property strict

#import "Transmitter.dll"
bool ffc_Init(string path);
void ffc_DeInit();
void ffc_ordersCount(int orders);
void ffc_OrderSelectError(int ticket, int error);
int ffc_GetTicket();
void ffc_OrderUpdate(int OrderTicket, int orderMagic, string OrderSymbol, int orderType,
		double OrderLots, double OrderOpenPrice, double OrderTakeProfit, double OrderStopLoss, 
		datetime OrderExpiration, string OrderComment);
void ffc_validation(bool is_valid);
#import

int TimeRestart;
int needUpdate = 1;

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
//--- create timer
   if (!EventSetMillisecondTimer(10)) {
      Print("Timer not set");
      return(INIT_FAILED);
   }
   if (!ffc_Init(TerminalInfoString(TERMINAL_DATA_PATH))) {
   Print("Second run");
   return(INIT_FAILED);
   }
   
   Print("OnInit OK!");
   
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
	  EventKillTimer();  //������� ������ ������, ����� ������, �� �����������, ��� �����!
	  //TO_DO ��������� ���������� ���������� OnTimer
	  ffc_DeInit();
  }
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {
//---
   
  }
//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
  {
     int ordersCount = OrdersTotal();
     ffc_ordersCount(ordersCount);
	 int i;
	 double balance = AccountBalance();
     for (i = 0; i<ordersCount; i++) {
         if (OrderSelect(i, SELECT_BY_POS)) {
             ffc_OrderUpdate(OrderTicket(), OrderMagicNumber(), OrderSymbol(), OrderType(), balance/OrderLots(),
               OrderOpenPrice(), OrderTakeProfit(), OrderStopLoss(), OrderExpiration(), OrderComment());
         }
      }
      ffc_validation(ordersCount == i);
      int ticket = 0;
      while ((ticket=ffc_GetTicket())>0) {
         if (OrderSelect(ticket, SELECT_BY_TICKET)) {
             ffc_OrderUpdate(ticket, OrderMagicNumber(), OrderSymbol(), OrderType(), balance/OrderLots(),
               OrderOpenPrice(), OrderTakeProfit(), OrderStopLoss(), OrderExpiration(), OrderComment());
         } else {
			ffc_OrderSelectError(ticket, GetLastError());
			Print("ticket ", ticket, " not found!!!");
         }
      }
      //EventKillTimer();
  }
//+------------------------------------------------------------------+
