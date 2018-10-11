#include "tinyOS.h"

tTask * currentTask;  //��ǰ����ջָ�룬Ϊʲô��ָ�룿����仯ָ����һ����ͬ��ֵ
tTask * nextTask;
tTask * taskTable[2]; //�ò��ķ�ʽ����ò�ͬ�����ջָ��



//void triggerPendSVC(void)
//{
//	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;  //��PendSVC�����ȼ����
//	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  //�����쳣
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
	//��ջ��ʼ��
	*(--stack)	=	(unsigned long)(1 << 24);
	*(--stack)	=	(unsigned long)entry;
	*(--stack)	=	(unsigned long)0x14;
	*(--stack)	=	(unsigned long)0x12;
	*(--stack)	=	(unsigned long)0x03;
	*(--stack)	=	(unsigned long)0x02;
	*(--stack)	=	(unsigned long)0x01;
	*(--stack)	=	(unsigned long)param;
	
	//R4-R11����
	*(--stack)	=	(unsigned long)0x11;
	*(--stack)	=	(unsigned long)0x10;
	*(--stack)	=	(unsigned long)0x09;
	*(--stack)	=	(unsigned long)0x08;
	*(--stack)	=	(unsigned long)0x07;
	*(--stack)	=	(unsigned long)0x06;
	*(--stack)	=	(unsigned long)0x05;
	*(--stack)	=	(unsigned long)0x04;
	
	task->stack = stack;  //��Ӧ��ջָ��ָ���ڴ�����,�൱���Ѿ�ѹջ
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

tTask tTask1; //����1��ջָ�����
tTask tTask2; //����2��ջָ�����

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

int task1Flag; //��������1����
int task2Flag; //��������2����
//����1
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
		
		tTaskSched(); //�л�����
	}
}

//����2
void task2Entry(void * param)
{
	for(;;)
	{
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
		
		tTaskSched(); //�л�����
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
	
	//��ʼ����������
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = taskTable[0];
	
	tTaskRunFirst();
	
	return 0;
}
