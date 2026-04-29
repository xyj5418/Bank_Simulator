#ifndef __TAMIAS__
#define __TAMIAS__



typedef struct {
	int TimeBusy; //sim_time apasxolisis tou tamia  柜员的忙碌模拟时间
	int TimeInactive; //sim_time adranias tou tamia  柜员的空闲模拟时间
	int ClientNumber; //arithmos pelatwn pou eksipiretise  已服务客户的数量
	int TimeLeft; //sim_time pou apomenei gia tin eksipiretisi tou pelati 客户剩余服务的模拟时间
	int Open_Cashier;
} Cashier;

/*Dilwsi sunartisewn*/

void CreateCashier(Cashier *cashier); //arxikopoiei to struct tou tamia  初始化柜员结构体

void CashierNewCustomer(Cashier *cashier); //prostheti pelati kai auksanei ta lepta kata 1  添加客户

void CashierSetTime(Cashier *cashier, int duration); //arxikopoiei enapominanta xrono  初始化剩余时间

void CashierNoWork(Cashier *cashier); //auksanei xrono adranias  增加空闲时间

void CashierBusy(Cashier *cashier); //auksanei xrono apasxolisis  增加忙碌时间

void ChangeCashierState(Cashier *cashier);

int CashierFree(Cashier cashier); //elegxei an einai diathesimos  检查是窗口是否空闲

int CashierGetClientNumber(Cashier *cashier); //epistrefei arithmo pelatwn  返回客户数量

int CashierGetTimeLeft(Cashier *cashier); //epistrefei enapomenon xrono  返回剩余时间

int CashierGetInactiveTime(Cashier *cashier); //epistrefei xrono adraneias  返回空闲时间

int CashierGetBusyTime(Cashier *cashier); //epistrefei xrono apasxolisis  返回忙碌时间

int CashierGetState(Cashier *cashier); //epistrefei tin katastasi tou tameia (anoixto/kleisto)  返回收银台状态（开启/关闭）

#endif
