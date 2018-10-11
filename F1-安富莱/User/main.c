#include "stm32f10x.h"
#include "bsp_timer.h"

uint16_t flag1 = 0;

void bsp_flag1(void);

int main(void)
{
	SCH_Add_Task(bsp_flag1,0,1000);
	bsp_InitTimer();
	
	while(1)
	{
		SCH_Dispatch_Tasks();
	}
}


void bsp_flag1(void)
{
	flag1 = ~flag1;
}

