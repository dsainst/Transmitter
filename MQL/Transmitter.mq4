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
bool ffc_Init();
void ffc_DeInit();
void ffc_ordersCount(int orders);
void ffc_OrderSelectError(int ticket, int error);
int ffc_GetTicket();
void ffc_OrderUpdate(int OrderTicket, int orderMagic, string OrderSymbol, int orderType,
		double OrderLots, double OrderOpenPrice, datetime OrderOpenTime,
		double OrderTakeProfit, double OrderStopLoss, double  OrderClosePrice, datetime  OrderCloseTime,
		datetime OrderExpiration, double  OrderProfit, double  OrderCommission, double  OrderSwap, string OrderComment);
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
   if (!ffc_Init()) {
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
	  EventKillTimer();  //—начала сносим таймер, потом деинит, не переставл€й, это важно!
	  //TO_DO проверить завершение выполнени€ OnTimer
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
     for (i = 0; i<ordersCount; i++) {
         if (OrderSelect(i, SELECT_BY_POS)) {
             ffc_OrderUpdate(OrderTicket(), OrderMagicNumber(), OrderSymbol(), OrderType(), AccauntBalance() \ OrderLots(),
               OrderOpenPrice(), OrderOpenTime(), OrderTakeProfit(), OrderStopLoss(),
               OrderClosePrice(), OrderCloseTime(), OrderExpiration(),
               OrderProfit(), OrderCommission(), OrderSwap(), OrderComment(),needUpdate);
         }
      }
      ffc_validation(ordersCount == i);
      int ticket = 0;
      while ((ticket=ffc_GetTicket())>0) {
         if (OrderSelect(ticket, SELECT_BY_TICKET)) {
             ffc_OrderUpdate(ticket, OrderMagicNumber(), OrderSymbol(), OrderType(), OrderLots(),
               OrderOpenPrice(), OrderOpenTime(), OrderTakeProfit(), OrderStopLoss(),
               OrderClosePrice(), OrderCloseTime(), OrderExpiration(),
               OrderProfit(), OrderCommission(), OrderSwap(), OrderComment());
         } else {
			ffc_OrderSelectError(ticket, GetLastError());
			Print("ticket ", ticket, " not found!!!");
         }
      }
      //EventKillTimer();
  }
//+------------------------------------------------------------------+
