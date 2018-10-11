#ifndef __BSP_TIME_H
#define __BSP_TIME_H
#include "stm32f10x.h"

typedef unsigned char tByte;
typedef unsigned int  tWord;
typedef struct
{
	void 	(*pTask)();  //指向任务的指针
	tWord 	Delay;       //延时，知道下一个函数的运动
	tWord   Period;      //连续运行之间的间隔
	tByte   RunMe;       //当任务需要运行的时候由调度器加1
} sTask;

/*********************软件定时器****************************************/
#define TMR_COUNT	4		/* 软件定时器的个数 （定时器ID范围 0 - 3) */
/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* 一次工作模式 */
	TMR_AUTO_MODE = 1		/* 自动定时工作模式 */
}TMR_MODE_E;

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct
{
	volatile uint8_t Mode;		/* 计数器模式，1次性 */
	volatile uint8_t Flag;		/* 定时到达标志  */
	volatile uint32_t Count;	/* 计数器 */
	volatile uint32_t PreLoad;	/* 计数器预装值 */
}SOFT_TMR;


////////////////////////////////////
tByte SCH_Add_Task(void (*pFuntion)(void),
					tWord DELAY,
					tWord PERIOD);

void bsp_InitTimer(void);

void SCH_Dispatch_Tasks(void);

#endif
