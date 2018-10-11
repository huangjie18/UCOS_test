/*
*********************************************************************************************************
*
*	模块名称 : 定时器模块
*	文件名称 : bsp_timer.c
*	版    本 : V1.0
*	说    明 : 配置systick定时器作为系统滴答定时器。缺省定时周期为1ms。
*
*				实现了多个软件定时器供主程序使用(精度1ms)， 可以通过修改 TMR_COUNT 增减定时器个数
*				实现了ms级别延迟函数（精度1ms） 和us级延迟函数
*				实现了系统运行时间函数（1ms单位）
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-02-01 armfly  正式发布
*		V1.1    2013-06-21 armfly  增加us级延迟函数 bsp_DelayUS
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp_timer.h"
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

#define SCH_MAX_TASKS   5
#define SystemCoreClock 72000000



sTask SCH_task_G[SCH_MAX_TASKS];  //建立的任务数

/* 这2个全局变量转用于 bsp_DelayMS() 函数 */
static volatile uint32_t s_uiDelayCount = 0;
static volatile uint8_t s_ucTimeOutFlag = 0;

/* 定于软件定时器结构体变量 */
static SOFT_TMR s_tTmr[TMR_COUNT];


/*
	全局运行时间，单位1ms
	最长可以表示 24.85天，如果你的产品连续运行时间超过这个数，则必须考虑溢出问题
*/
__IO int32_t g_iRunTime = 0;


/*
*******************************************************************************************
* 函 数 名: bsp_InitTimer
* 功能说明: 配置 systick 中断，并初始化软件定时器变量
* 形 参: 无
* 返 回 值: 无
*******************************************************************************************
*/
void bsp_InitTimer(void)
{
	uint8_t i;
	
	//清零所有的软件定时器
	for(i = 0;i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TMR_ONCE_MODE;
		
	}
	
	//配置systick中断周期为1ms,并启动systick中断
	SysTick_Config(SystemCoreClock/1000);
}

/*
*********************************************************************************************************
*	函 数 名: SCH_Update(void)
*	功能说明: 调度器的刷新函数，每个时标中断执行一次。在嘀嗒定时器中断里面执行。
*			  当刷新函数确定某个任务要执行的时候，将RunMe加1，要注意的是刷新任务
*			  不执行任何函数，需要运行的任务有调度函数激活。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SCH_Update(void)
{
	tByte index;
	
	/*注意计数单位是时标，不是毫秒*/
	for(index = 0; index < SCH_MAX_TASKS;index++)
	{
		//检测这里是否有任务
		if(SCH_task_G[index].pTask)
		{
			//如果该任务延时时间到了，RunMe置1标志任务就绪
			if(SCH_task_G[index].Delay == 0)
			{
				/*任务需要运行 将RunMe置1*/
				SCH_task_G[index].RunMe += 1;
				
				if(SCH_task_G[index].Period)
				{
					/*调度周期性的任务再次执行*/
					SCH_task_G[index].Delay = SCH_task_G[index].Period;
				}
			}
			else
			{
				/*还有准备好运行*/
				SCH_task_G[index].Delay -= 1;
			}
		}
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_RunPer1ms
*	功能说明: 该函数每隔1ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些需要周期性处理的
*			事务可以放在此函数。比如：触摸坐标扫描。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer1ms(void)
{
	SCH_Update();
}

/*
*********************************************************************************************************
*	函 数 名: bsp_RunPer10ms
*	功能说明: 该函数每隔10ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些需要周期性处理
*			的事务可以放在此函数。比如：按键扫描、蜂鸣器鸣叫控制等。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{
	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SoftTimerDec
*	功能说明: 每隔1ms对所有定时器变量减1。必须被SysTick_ISR周期性调用。
*	形    参:  _tmr : 定时器变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->Count > 0)
	{
		/* 如果定时器变量减到1则设置定时器到达标志 */
		if (--_tmr->Count == 0)
		{
			_tmr->Flag = 1;

			/* 如果是自动模式，则自动重装计数器 */
			if(_tmr->Mode == TMR_AUTO_MODE)
			{
				_tmr->Count = _tmr->PreLoad;
			}
		}
	}
}
/*
*********************************************************************************************************
*	函 数 名: SysTick_ISR
*	功能说明: SysTick中断服务程序，每隔1ms进入1次
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void SysTick_ISR(void)
{
	static uint8_t s_count = 0;
	uint8_t i;
	
	/* 每隔1ms进来1次 （仅用于 bsp_DelayMS） */
	if(s_uiDelayCount > 0) //用来做延时函数的标志位
	{
		if(--s_uiDelayCount == 0)
		{
			s_ucTimeOutFlag = 1;  //标志延时时间到
		}
	}
	
	/* 每隔1ms，对软件定时器的计数器进行减一操作 */
	//TMR_COUNT表示有多少个软件定时器
	for(i = 0; i < TMR_COUNT; i++)
	{
		bsp_SoftTimerDec(&s_tTmr[i]);  //软件定时器减1ms
	}
	
	/* 全局运行时间每1ms增1 */
	g_iRunTime++;
	if(g_iRunTime == 0x7FFFFFFF) /* 这个变量是 int32_t 类型，最大数为 0x7FFFFFFF */
	{
		g_iRunTime = 0;
	}
	
	bsp_RunPer1ms();		/* 每隔1ms调用一次此函数，调用SCH_Update */
	
	if(++s_count >= 10)
	{
		s_count = 0;
		bsp_RunPer10ms();  /* 每隔10ms调用一次此函数 */
	}
	
}


/*
*********************************************************************************************************
*	函 数 名: SCH_Add_Task
*	功能说明: 添加任务。
*	形    参：void (*pFuntion)(void); tWord DELAY; tWord PERIOD
*	返 回 值: 返回任务的ID号
*   使用说明：
*	（1）SCH_Add_Task(DOTASK,1000,0) DOTASK是函数的运行地址，1000是1000个时标以后开始运行，只运行一次；
*	（2）Task_ID = SCH_Add_Task(DOTASK,1000,0); 将任务标示符保存 以便以后删除任务 
*	（3）SCH_Add_Task(DOTASK,0,1000); 每个1000个时标周期性的运行一次；
*********************************************************************************************************
*/
#define  RETURN_ERROR                 0x00;
#define  RETURN_NORMOL                0x01;
#define  ERROR_SCH_CANOT_DELETE_TASK  0x02;
#define  ERROR_SCH_TOO_MANY_TASKS	  0x03;

tByte Error_code_G; //错误信息

tByte SCH_Add_Task(void (*pFuntion)(void),
					tWord DELAY,
					tWord PERIOD)
{
	tByte index = 0; /*首先在队列中找到一个空隙，（如果有的话）*/
	//pTask还没赋值表示可以添加任务到该成员
	while((SCH_task_G[index].pTask != 0) && (index < SCH_MAX_TASKS))
	{
		index++;
	}
	/*超过最大的任务数目 则返错误信息*/
	if(index == SCH_MAX_TASKS)
	{
		Error_code_G = ERROR_SCH_TOO_MANY_TASKS;/*设置全局错误变量*/
		return SCH_MAX_TASKS;
	}
	
	SCH_task_G[index].pTask = pFuntion;	/*运行到这里说明申请的任务块成功*/
	SCH_task_G[index].Delay = DELAY;
	SCH_task_G[index].Period = PERIOD;
	SCH_task_G[index].RunMe =0;
	
	return index;					   /*返回任务的位置，以便于以后删除*/
	
}

/*
*********************************************************************************************************
*	函 数 名: SCH_Task_Delete
*	功能说明: 删除任务。
*	形    参：tByte index
*	返 回 值: 是否删除成功
*********************************************************************************************************
*/
tByte SCH_Task_Delete(tByte index)
{
	tByte Return_code;
	
	/*这里没有任务*/
	if(SCH_task_G[index].pTask == 0)				   
	{
		/*设置全局错误变量*/
		Error_code_G = ERROR_SCH_CANOT_DELETE_TASK;
		Return_code  = RETURN_ERROR;
	}
	else
	{	
		Return_code  = RETURN_NORMOL;		
	}

	SCH_task_G[index].pTask = 0x0000;
	SCH_task_G[index].Delay = 0;
	SCH_task_G[index].Period = 0;
	SCH_task_G[index].RunMe =0;
	
	return Return_code;		/*返回状态*/
}
/*
*********************************************************************************************************
*	函 数 名: SCH_Dispatch_Tasks
*	功能说明: 在主任务里面执行的调度函数，顺序执行没有优先级之分
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SCH_Dispatch_Tasks(void)
{
	tByte index;
	for(index = 0; index < SCH_MAX_TASKS; index++)
	{
		if(SCH_task_G[index].RunMe >0)
		{
			/*执行任务 */
			(*SCH_task_G[index].pTask)();    

			/* 执行任务完成后，将RunMe减一 */
			SCH_task_G[index].RunMe -= 1;

			/*如果是单次任务的话，则将任务删除 */	  
			if(SCH_task_G[index].Period == 0) 
			{		
				SCH_Task_Delete(index);
			}
		}
	}
}