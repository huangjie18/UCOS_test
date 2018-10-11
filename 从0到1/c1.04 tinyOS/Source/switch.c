#include "tinyOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL    0xE000ED04
#define NVIC_PENDSVSET   0X10000000
#define NVIC_SYSPRI2     0XE000ED22
#define NVIC_PENDSV_PRI  0X000000FF

#define MEM32(addr)  *(volatile unsigned long *)(addr)
#define MEM8(addr)   *(volatile unsigned char *)(addr)


//重要任务切换核心
__asm void PendSV_Handler (void)
{
//	IMPORT blockPtr
//	
//	LDR R0, =blockPtr  ;//得到blockPtr指针变量的地址
//	LDR R0, [R0]       ;//得到blockPtr所指向的BlockType_t结构体变量block的地址也就是stackPtr的地址
//	LDR R0, [R0]       ;//得到block里面的指针所指地址也就是&stackBuffer[1024]的地址
//	
//	// 保存寄存器
//	STMDB R0!, {R4-R11}
//	
//	LDR  R1, =blockPtr
//	LDR  R1, [R1]       ;//得到blockPtr所指向的BlockType_t结构体变量block的地址也就是stackPtr的地址
//	STR  R0, [R1]       ;//因为进行了压栈，所以要修改当前的栈指向地址
//	
//	// 修改部分寄存器，用于测试
//	ADD  R4, R4, #1
//	ADD  R5, R5, #1
//	
//	// 恢复寄存器
//	LDMIA R0!, {R4-R11}
//	
//	// 异常返回
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
	LDR	R0,	=currentTask   ;//R0 = currentTask变量的地址
	LDR	R1,	=nextTask
	LDR	R2,	[R1]
	STR	R2,	[R0]			;//此时currentTask = nextTask
	
	LDR	R0,	[R2]
	LDMIA	R0!, {R4-R11}
	
	MSR	PSP, R0
	ORR LR, LR, #0x04
	BX	LR

	
}

void tTaskRunFirst(void)
{
	__set_PSP(0);  //设置PSP栈指针为0
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;  //令PendSVC的优先级最低
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  //触发异常
}

void tTaskSwitch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  //触发异常
}