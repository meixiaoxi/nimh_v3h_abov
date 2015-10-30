#include "nimh.h"



extern u32 shortTick;

void delay_ms(u16 nus)
{
    unsigned int i , j;
    for(i=0;i<nus;i++)
   {
   	ClrWdt();
	for(j=0;j<1000;j++);
    }
}

void delay_us(u16 nus)
{
	u16 i;
	for(i=0;i<nus;i++);
	ClrWdt();
}
u32 getSysTick()
{
	u32 tempTick;

	EA = 0;
	tempTick = shortTick;
	EA = 1;

	return tempTick;
}

u32 getDiffTickFromNow(u32 ttick)
{
	u32 ticknow =  getSysTick();

	if(ttick <= ticknow)
		return (ticknow - ttick); //getDiffTick(ttick, ticknow);
	else
		return (0xFFFFFFFF-ttick+ticknow);
}

