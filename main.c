#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <shlobj.h>
#include "Queue.h"
#include "Clients.h"
#include "Cashiers.h"

#define CASHIERS 10

int main(void)
{	Queue      		queue;				 			/*客户排队*/
	float			arrival_prob; 					/*客户前来办理业务的概率*/
	unsigned int	max_service_time;				/*一个客户的最长模拟服务时长*/ 
	unsigned int	bank_working_hours; 			/*银行接待客户的运营时长*/
	
	unsigned int	sim_time;				 			/*模拟时钟*/
	unsigned int	wait_time;	 			/*模拟总等待时长*/
	Client		Client;		 				/*排队的客户*/
	float			average_time;		 			/*平均等待时长*/
	float			random_arrival;
	
	Cashier			cashier[CASHIERS]; 				//每个窗口的结构体的数组
	int next_cashier_spot = 0;					//将会服务下一个客户的窗口
	int duration;									//客户的模拟服务时长
	int i,j;
	int clients_number = 0;						//进入银行的客户数量
	int next_cashier = 0;							//如果排队太长，就会打开下一个窗口
	int flag = 0;
	int choice;									//用于选择模拟策略
	int free_cashier[CASHIERS];						//空闲窗口的数组
	int busy_cashier = 1;							//如果窗口繁忙返回1
	int count_used_cashier = 0;                    //工作过的窗口数量，用于计算窗口利用率并写入文件进行保存

	float count_total = 0;                         //利用率总和，用于计算窗口利用率并写入文件保存
	char desktop_path[MAX_PATH];                  //用于数据记录保存至桌面
	char time_str[64];                         //为时间记录创建字符数组
	time_t	now = time(NULL);              //每次数据记录的时间


	do {
		printf("======请选择模拟策略：======\n");
		printf("1, 按需开启窗口\n");
		printf("2, 直接开启所有窗口\n");
		printf("请选择：");
		scanf("%d", &choice);

		if (choice != 1 && choice != 2)
			printf("请输入1或者2");
	} while (choice != 1 && choice != 2);

	printf("输入银行工作时间 (>=0), 客户到来的概率 (0<且<1) 以及最长受理时长 (>=1) )\n");
	scanf("%u %f %u",&bank_working_hours,&arrival_prob,&max_service_time);
	//getchar();
	
	printf("银行的保持营业的时长为 %4u 分钟\n",bank_working_hours);
	printf("客户前来办理业务的概率: %4.2f.\n",arrival_prob);
	printf("最长服务时长: %d 分钟\n",max_service_time);
	fflush(stdout);

	CreateQueue(&queue);
	sim_time = 0;
	
	for(i=0; i<CASHIERS; i++){
		CreateCashier(&cashier[i]); 				//初始化每一个窗口
	}
	
	
	if(choice == 1)
	{								//如果选择策略1
		ChangeCashierState(&cashier[0]);				//先只打开一个窗口，之后按需增加窗口
	}
	else
	{											//选择策略2
		for(i = 0; i < CASHIERS; i++)
		{
			ChangeCashierState(&cashier[i]);			//直接打开每一个窗口，分配客户
		}
		for(i = 0; i < CASHIERS; i++)
		{
			free_cashier[i] = i;						//为窗口可用情况开一个整型数组
		}
	}

	wait_time = 0;
	srand((unsigned)time(NULL));

	while( (sim_time < bank_working_hours || !QueueEmpty(queue)) || busy_cashier == 1 ) //当银行开门并且要排队并且窗口占用时运行
	{
		busy_cashier = 0;

		random_arrival = (float)rand()/(float)RAND_MAX;
		
		if ( random_arrival < arrival_prob && sim_time < bank_working_hours)
		{
			clients_number++;						//增加进入银行的客户人数
			ClientSetEnterTime(&Client, sim_time);
			duration = rand()%max_service_time + 1; //模拟服务时间
			ClientSetServiceTime(&Client, duration);	
			
			if (!QueueAddition(&queue, Client))
			{
                printf("Queue is small! Simulation stops! \n");
                getchar();
                return 0;
        	}
    	}
    	
    	if(choice == 1)                                           //在第一种策略下
		{
    		if(QueueSize(queue) > bank_working_hours/CASHIERS)
			{					                                        //如果队伍超过了一定长度
    			if(next_cashier <= CASHIERS)  
				{								                         //并且有空余窗口
    				ChangeCashierState(&cashier[next_cashier]);				//打开新的窗口
    				next_cashier++;										//修改更新下一个准备打开的窗口的号码信息
    			}
    		}
			else if (QueueSize(queue) < bank_working_hours/(CASHIERS*2))
			{                                                    			//如果客户队伍减少			
					next_cashier--;											//减少打开窗口，更新打开窗口的号码
					ChangeCashierState(&cashier[next_cashier]);			    //关闭上一个打开的窗口
    		}
    	
    		/*寻找接待下一个客户的窗口*/
    		for(i = next_cashier_spot; i < next_cashier - 1; i++)
			{
				if (CashierFree(cashier[i]) == 1)
				{
					next_cashier_spot = i;
					flag = 1;                      //如果没有空余窗口，flag赋值为一
					break;
				}
    		}
    		if(!flag)
			{													//从头开始再找一遍空余的窗口
					next_cashier_spot = 0;	
    		}
    		
    		/* 增加每一个窗口的忙碌时间和空闲时间*/
    		for(i = 0; i < next_cashier - 1; i++)
			{
	    		if(CashierFree(cashier[i]))
				{
	    			CashierNoWork(&cashier[i]);
	    		}
				else
				{
	    			CashierBusy(&cashier[i]);
	    		}
	    	}
	    	
    		flag = 0;
    		for(i = 0; i < next_cashier -1; i++)
			{
				if(CashierFree(cashier[i]) == 0)
				{
					busy_cashier = 1;
					break;
				}
			}
    	}
		else if(choice == 2)
		{
    		/* 增加每个窗口的忙碌时间和空闲时间*/
    		for(i = 0; i < CASHIERS; i++)
			{
	    		if(CashierFree(cashier[i]))
				{
	    			CashierNoWork(&cashier[i]);
	    		}
				else
				{
	    			CashierBusy(&cashier[i]);
	    		}
	    	}
	    	
	    	/* 用新的空闲窗口更新数组信息 */
    		for(i = 0; i < CASHIERS; i++)
			{
    			if(CashierFree(cashier[i]) == 1)
				{
    				for(j = 0; j < CASHIERS; j++)
					{
    					if(free_cashier[j] == i )
    						break;
    						
    					if(free_cashier[j] == -1)
						{
    						free_cashier[j] = i;
    						break;
    					}
    				}
    			}
    		}
    		/* 选择第一个空余的窗口接待下一个客户 */
    		if(free_cashier[0] != -1)
			{
	    		next_cashier_spot = free_cashier[0];
	    		for(i = 1; i < CASHIERS; i++)
				{
	    			free_cashier[i-1] = free_cashier[i]; 			//把每个窗口在数组中向左移动一位
	    		}
	    		free_cashier[CASHIERS - 1] = -1;					//设定-1使最后一个窗口空出来
    		}
    		for(i = 0; i < CASHIERS; i++)
			{
				if(CashierFree(cashier[i]) == 0)
				{
					busy_cashier = 1;
					break;
				}
			}
    	}
    	

		if ( CashierFree(cashier[next_cashier_spot]))							/*如果有一个空余窗口*/
			if (!QueueEmpty(queue))												/*并且有一个客户*/
			{	
				QueueLeave(&queue,&Client);								/*客户前往窗口*/
				wait_time += sim_time - ClientGetEnterTime(&Client);	/*计算模拟等待时间*/
				CashierNewCustomer(&cashier[next_cashier_spot]);
				CashierSetTime(&cashier[next_cashier_spot], ClientGetServiceTime(&Client));
			}
		
		sim_time++; /*模拟时钟更新*/
		
	} 

	if (CashierGetClientNumber(&cashier[next_cashier_spot]) == 0)
		average_time = 0.0;
	else
		average_time = ((float)wait_time)/((float)clients_number);

	printf("\n受理的客户数量 : %d\n",clients_number);
	printf("平均等待时间 : %4.2f 分钟\n",average_time);
	printf("总工作时间: %d 分钟\n",sim_time);
	printf("营业结束后处理剩余客户所用时间: %d 分钟\n", sim_time - bank_working_hours);
	
	for(i = 0; i < CASHIERS; i++)
	{
		
		printf("窗口号 : %d\n", i+1);
		printf("\t客户数量 : %d\n", cashier[i].ClientNumber);
		printf("\t工作时间 : %d 分钟\n", cashier[i].TimeBusy);
		printf("\t停止时间 : %d 分钟\n", cashier[i].TimeInactive);

		if (cashier[i].ClientNumber >= 1)
		{
			count_used_cashier = 1;
			count_total += count_used_cashier * (cashier[i].TimeBusy / (sim_time - cashier[i].TimeInactive) );
		}
	}


	//数据记录自动保存到桌面，并且带有每次记录的时间
	struct tm* local_time = localtime(&now);
	//格式化时间：年月日 时分秒
	strftime(time_str, sizeof(time_str), "%Y-%m-%d  %H:%M:%S", local_time);
	//获取桌面路径
	SHGetSpecialFolderPath(NULL, desktop_path, CSIDL_DESKTOPDIRECTORY, 0);
	//拼接文件名
	char full_path[MAX_PATH];
	sprintf(full_path, "%s\\simul-report.txt", desktop_path);

	FILE* fp;
	fp = fopen(full_path, "a+");              //追加模式，自动创建文件，并且再次运行不会删除原纪录
	if (fp != NULL)
	{
		fprintf(fp, "====================================\n");
		fprintf(fp, "\n模拟时间：%s\n", time_str);
		fprintf(fp, "银行排队模拟结果如下：\n");
		fprintf(fp, "银行的保持营业的时长为 %4u 分钟\n", bank_working_hours);
		fprintf(fp,  "客户前来办理业务的概率: %4.2f.\n", arrival_prob);
		fprintf(fp, "最长服务时长: %d 分钟\n", max_service_time);
		//fprintf(fp, "窗口利用率为：%.2f\n", count_total / CASHIERS);
		fprintf(fp, "\n受理的客户数量 : %d\n", clients_number);
		fprintf(fp, "平均等待时间 : %4.2f 分钟\n", average_time);
		fprintf(fp, "总工作时间: %d 分钟\n", sim_time);
		fprintf(fp, "营业结束后处理剩余客户所用时间: %d 分钟\n", sim_time - bank_working_hours);

		printf("输入完成");
	}
	else
	{
		printf("Open file error!");
		return -1;
	}

	fclose(fp);

	return 0;
}
