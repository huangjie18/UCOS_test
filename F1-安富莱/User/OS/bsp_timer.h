#ifndef __BSP_TIME_H
#define __BSP_TIME_H
#include "stm32f10x.h"

typedef unsigned char tByte;
typedef unsigned int  tWord;
typedef struct
{
	void 	(*pTask)();  //ָ�������ָ��
	tWord 	Delay;       //��ʱ��֪����һ���������˶�
	tWord   Period;      //��������֮��ļ��
	tByte   RunMe;       //��������Ҫ���е�ʱ���ɵ�������1
} sTask;

/*********************�����ʱ��****************************************/
#define TMR_COUNT	4		/* �����ʱ���ĸ��� ����ʱ��ID��Χ 0 - 3) */
/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* һ�ι���ģʽ */
	TMR_AUTO_MODE = 1		/* �Զ���ʱ����ģʽ */
}TMR_MODE_E;

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef struct
{
	volatile uint8_t Mode;		/* ������ģʽ��1���� */
	volatile uint8_t Flag;		/* ��ʱ�����־  */
	volatile uint32_t Count;	/* ������ */
	volatile uint32_t PreLoad;	/* ������Ԥװֵ */
}SOFT_TMR;


////////////////////////////////////
tByte SCH_Add_Task(void (*pFuntion)(void),
					tWord DELAY,
					tWord PERIOD);

void bsp_InitTimer(void);

void SCH_Dispatch_Tasks(void);

#endif
