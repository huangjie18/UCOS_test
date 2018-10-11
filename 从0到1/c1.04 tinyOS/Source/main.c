#include "tinyOS.h"

tTask * currentTask;  //当前任务栈指针，为什么用指针？方便变化指向下一个不同的值
tTask * nextTask;
tTask * taskTable[2]; //用查表的方式来获得不同任务的栈指针



//void triggerPendSVC(void)
//{
//	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;  //令PendSVC的优先级最低
//	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  //触发异常
//}

typedef struct _BlockType_t
{
	unsigned long * stackPtr;
	
}BlockType_t;

BlockType_t * blockPtr;


void delay(int count)
{
	while(--count > 0);
}



void tTaskInit(tTask * task,void (*entry)(void *),void * param, tTaskStack *stack)
{
	//堆栈初始化
	*(--stack)	=	(unsigned long)(1 << 24);
	*(--stack)	=	(unsigned long)entry;
	*(--stack)	=	(unsigned long)0x14;
	*(--stack)	=	(unsigned long)0x12;
	*(--stack)	=	(unsigned long)0x03;
	*(--stack)	=	(unsigned long)0x02;
	*(--stack)	=	(unsigned long)0x01;
	*(--stack)	=	(unsigned long)param;
	
	//R4-R11保存
	*(--stack)	=	(unsigned long)0x11;
	*(--stack)	=	(unsigned long)0x10;
	*(--stack)	=	(unsigned long)0x09;
	*(--stack)	=	(unsigned long)0x08;
	*(--stack)	=	(unsigned long)0x07;
	*(--stack)	=	(unsigned long)0x06;
	*(--stack)	=	(unsigned long)0x05;
	*(--stack)	=	(unsigned long)0x04;
	
	task->stack = stack;  //对应于栈指针指向内存数组,相当于已经压栈
}

void tTaskSched()
{
	if(currentTask == taskTable[0])
	{
		nextTask = taskTable[1];
	}
	else
	{
		nextTask = taskTable[0];
	}
	
	tTaskSwitch();
}

tTask tTask1; //任务1的栈指针变量
tTask tTask2; //任务2的栈指针变量

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

int task1Flag; //用于任务1测试
int task2Flag; //用于任务2测试
//任务1
void task1Entry(void * param)
{
	unsigned long value = *(unsigned long *)param;
	value++;
	for(;;)
	{
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
		
		tTaskSched(); //切换任务
	}
}

//任务2
void task2Entry(void * param)
{
	for(;;)
	{
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
		
		tTaskSched(); //切换任务
	}
}

int main()
{
	unsigned long i = 0;
	i++;
	if(i == 1)
	{
		i++;
	}
	tTaskInit(&tTask1, task1Entry,(void *)0x11111111, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, 0, &task2Env[1024]);
	
	//初始化任务数组
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = taskTable[0];
	
	tTaskRunFirst();
	
	return 0;
}
