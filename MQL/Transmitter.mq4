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
int ffc_OrderSelectError(int ticket);
int ffc_GetTicket();
void ffc_OrderUpdate(int OrderTicket, int orderMagic, string OrderSymbol, int orderType,
		double OrderLots, double OrderOpenPrice, datetime OrderOpenTime,
		double OrderTakeProfit, double OrderStopLoss, double  OrderClosePrice, datetime  OrderCloseTime,
		datetime OrderExpiration, double  OrderProfit, double  OrderCommission, double  OrderSwap, string OrderComment, int TimeRestart);
void ffc_validation(int orders);
#import

int TimeRestart = 0;
int needUpdate;

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
//--- create timer
   if (!EventSetMillisecondTimer(100)) {
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
  ffc_DeInit();
//--- destroy timer
   EventKillTimer();
      
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
      int minute = Minute();
     needUpdate = minute?minute:60 - TimeRestart;
     if (needUpdate)
     TimeRestart = minute;
     int ordersCount = OrdersTotal();
     Print("count - ",ordersCount);
     ffc_ordersCount(ordersCount);
     printf(IntegerToString(ordersCount));
      for (int i = 0; i<ordersCount; i++) {
         if (OrderSelect(i, SELECT_BY_POS)) {
             ffc_OrderUpdate(OrderTicket(), OrderMagicNumber(), OrderSymbol(), OrderType(), OrderLots(),
               OrderOpenPrice(), OrderOpenTime(), OrderTakeProfit(), OrderStopLoss(),
               OrderClosePrice(), OrderCloseTime(), OrderExpiration(),
               OrderProfit(), OrderCommission(), OrderSwap(), OrderComment(),needUpdate);
         }
      }
      ffc_validation(ordersCount == OrdersTotal());
      int ticket = 0;
      while ((ticket=ffc_GetTicket())>0) {
         if (OrderSelect(ticket, SELECT_BY_TICKET)) {
             ffc_OrderUpdate(ticket, OrderMagicNumber(), OrderSymbol(), OrderType(), OrderLots(),
               OrderOpenPrice(), OrderOpenTime(), OrderTakeProfit(), OrderStopLoss(),
               OrderClosePrice(), OrderCloseTime(), OrderExpiration(),
               OrderProfit(), OrderCommission(), OrderSwap(), OrderComment(),needUpdate);
         } else {
            printf("ticket_id = " + IntegerToString(ffc_OrderSelectError(ffc_GetTicket())));
         }
      }
      //EventKillTimer();
  }
//+------------------------------------------------------------------+
