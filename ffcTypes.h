#pragma once

#define SYMBOL_LENGTH	16
#define COMMENT_LENGTH	32
#define MAX_ORDER_COUNT	200


#pragma pack(push,1)
struct FfcOrder
{
	int			ticket;
	int			magic;		//Provider
	wchar_t		symbol[SYMBOL_LENGTH];
	int			type;
	double		lots;		// Depo/Lot
	double		openprice;
	//__time64_t	opentime;
	double		tpprice;
	double		slprice;
	//double		closeprice;
	//__time64_t	closetime;
	__time64_t	expiration;
	//double		profit;
	//double		comission;
	//double		swap;
	//wchar_t		comment[COMMENT_LENGTH];
};
#pragma pack(pop,1)
#pragma pack(push,1)
struct FfcMsg
{
	int		validation;
	int		ordersCount;
	FfcOrder orders[MAX_ORDER_COUNT];
};
#pragma pack(pop,1)