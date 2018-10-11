/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                             (c) Copyright 1998-2004, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
*                                            WIN32 Sample Code
*
* File : APP.C
* By   : Eric Shufro
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                                CONSTANTS
*********************************************************************************************************
*/

//#define  TASK_STK_SIZE    128
//#define  TASK_START_PRIO    5
//
///*
//*********************************************************************************************************
//*                                                VARIABLES
//*********************************************************************************************************
//*/
//
//OS_STK        AppStartTaskStk[TASK_STK_SIZE];
//
///*
//*********************************************************************************************************
//*                                            FUNCTION PROTOTYPES
//*********************************************************************************************************
//*/
//
//static  void  AppStartTask(void *p_arg);
//
//#if OS_VIEW_MODULE > 0
//static  void  AppTerminalRx(INT8U rx_data);
//#endif
//
///*
//*********************************************************************************************************
//*                                                _tmain()
//*
//* Description : This is the standard entry point for C++ WIN32 code.  
//* Arguments   : none
//*********************************************************************************************************
//*/
//
//void main(int argc, char *argv[])
//{
//	INT8U  err;
//
//
//#if 0
//    BSP_IntDisAll();                       /* For an embedded target, disable all interrupts until we are ready to accept them */
//#endif
//
//    OSInit();                              /* Initialize "uC/OS-II, The Real-Time Kernel"                                      */
//
//    OSTaskCreateExt(AppStartTask,
//                    (void *)0,
//                    (OS_STK *)&AppStartTaskStk[TASK_STK_SIZE-1],
//                    TASK_START_PRIO,
//                    TASK_START_PRIO,
//                    (OS_STK *)&AppStartTaskStk[0],
//                    TASK_STK_SIZE,
//                    (void *)0,
//                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
//
//#if OS_TASK_NAME_SIZE > 11
//    OSTaskNameSet(APP_TASK_START_PRIO, (INT8U *)"Start Task", &err);
//#endif
//
//#if OS_TASK_NAME_SIZE > 14
//    OSTaskNameSet(OS_IDLE_PRIO, (INT8U *)"uC/OS-II Idle", &err);
//#endif
//
//#if (OS_TASK_NAME_SIZE > 14) && (OS_TASK_STAT_EN > 0)
//    OSTaskNameSet(OS_STAT_PRIO, "uC/OS-II Stat", &err);
//#endif
//
//    OSStart();                             /* Start multitasking (i.e. give control to uC/OS-II)                               */
//}
///*$PAGE*/
///*
//*********************************************************************************************************
//*                                          STARTUP TASK
//*
//* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
//*               initialize the ticker only once multitasking has started.
//* Arguments   : p_arg   is the argument passed to 'AppStartTask()' by 'OSTaskCreate()'.
//* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
//*                  used.  The compiler should not generate any code for this statement.
//*               2) Interrupts are enabled once the task start because the I-bit of the CCR register was
//*                  set to 0 by 'OSTaskCreate()'.
//*********************************************************************************************************
//*/
//
//void  AppStartTask (void *p_arg)
//{
//    p_arg = p_arg;
//
//#if 0
//    BSP_Init();                                  /* For embedded targets, initialize BSP functions                             */
//#endif
//
//
//#if OS_TASK_STAT_EN > 0
//    OSStatInit();                                /* Determine CPU capacity                                                     */
//#endif
//    
//    while (TRUE)                                 /* Task body, always written as an infinite loop.                             */
//	{       		
//		OS_Printf("Delay 1 second and print\n");  /* your code here. Create more tasks, etc.                                    */
//        OSTimeDlyHMSM(0, 0, 1, 0);       
//    }
//}




#define  TASK_STK_SIZE  512
OS_STK  MyTaskStk[TASK_STK_SIZE];
OS_STK  YouTackStk[TASK_STK_SIZE];


INT16S  key;
INT8U   x = 0, y = 0;

void MyTask(void *pdata);
void YouTask(void *pdata);

void main(void)
{
	char * s_M = "M";
	OSInit();
	OSTaskCreate(MyTask, s_M, &MyTaskStk[TASK_STK_SIZE - 1], 0);
	OSStart();

}

void MyTask(void *pdata)
{
	char * s_Y = "Y";
	char i = 1;
	pdata = pdata;
	OS_ENTER_CRITICAL();
	OS_EXIT_CRITICAL();
	OSStatInit();
	OSTaskCreate(YouTask, s_Y, &YouTackStk[TASK_STK_SIZE - 1], 2);

	while (1)
	{
		if (i > 5)
		{
			while (OSTaskDelReq(2) != OS_TASK_NOT_EXIST)
			{
				OS_Printf("MyTask:我要求YouTask自己删除自己!\n");
				OSTimeDly(1);
			}
			
		}
		
		i++;
		OS_Printf("print:%s\n", pdata);
		OSTimeDlyHMSM(0, 0, 1, 0);//延时1s
	}
}

void YouTask(void * pdata)
{
	char i = 0;
	char * s1 = "YouTask:我必须要删除我自己了！";
	char * s2 = "YouTask:我已经删除我自己了！";
	pdata = pdata;
	for (;;)
	{

			if (OSTaskDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
			{
				OS_Printf("print:%s\n", s1);
				OS_Printf("print:%s\n", s2);
				OSTaskDel(OS_PRIO_SELF);
				
			}

		OS_Printf("print:%s\n", pdata);
		OSTimeDlyHMSM(0, 0, 1, 0);//延时1s
	}
}
