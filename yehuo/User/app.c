#include "stdint.h"

unsigned int flag1;
unsigned int flag2;

void delay(uint32_t count)
{
	for(;count!=0;count--);
}

int main(void)
{
	for(;;)
	{
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
	}
}