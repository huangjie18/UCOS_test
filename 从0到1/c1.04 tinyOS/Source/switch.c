#include "tinyOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL    0xE000ED04
#define NVIC_PENDSVSET   0X10000000
#define NVIC_SYSPRI2     0XE000ED22
#define NVIC_PENDSV_PRI  0X000000FF

#define MEM32(addr)  *(volatile unsigned long *)(addr)
#define MEM8(addr)   *(volatile unsigned char *)(addr)


//��Ҫ�����л�����
__asm void PendSV_Handler (void)
{
//	IMPORT blockPtr
//	
//	LDR R0, =blockPtr  ;//�õ�blockPtrָ������ĵ�ַ
//	LDR R0, [R0]       ;//�õ�blockPtr��ָ���BlockType_t�ṹ�����block�ĵ�ַҲ����stackPtr�ĵ�ַ
//	LDR R0, [R0]       ;//�õ�block�����ָ����ָ��ַҲ����&stackBuffer[1024]�ĵ�ַ
//	
//	// ����Ĵ���
//	STMDB R0!, {R4-R11}
//	
//	LDR  R1, =blockPtr
//	LDR  R1, [R1]       ;//�õ�blockPtr��ָ���BlockType_t�ṹ�����block�ĵ�ַҲ����stackPtr�ĵ�ַ
//	STR  R0, [R1]       ;//��Ϊ������ѹջ������Ҫ�޸ĵ�ǰ��ջָ���ַ
//	
//	// �޸Ĳ��ּĴ��������ڲ���
//	ADD  R4, R4, #1
//	ADD  R5, R5, #1
//	
//	// �ָ��Ĵ���
//	LDMIA R0!, {R4-R11}
//	
//	// �쳣����
//	BX LR

	IMPORT	currentTask
	IMPORT	nextTask
	
	MRS	R0,	PSP
	CBZ	R0,	PendSV_Handler_Nosave
	STMDB R0!, {R4-R11}
	LDR	R1, =currentTask
	LDR R1,[R1]
	STR R0,[R1]
	
PendSV_Handler_Nosave
	LDR	R0,	=currentTask   ;//R0 = currentTask�����ĵ�ַ
	LDR	R1,	=nextTask
	LDR	R2,	[R1]
	STR	R2,	[R0]			;//��ʱcurrentTask = nextTask
	
	LDR	R0,	[R2]
	LDMIA	R0!, {R4-R11}
	
	MSR	PSP, R0
	ORR LR, LR, #0x04
	BX	LR

	
}

void tTaskRunFirst(void)
{
	__set_PSP(0);  //����PSPջָ��Ϊ0
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;  //��PendSVC�����ȼ����
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  //�����쳣
}

void tTaskSwitch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  //�����쳣
}