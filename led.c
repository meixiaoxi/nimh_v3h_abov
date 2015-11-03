#include "nimh.h"

extern u8 gBatStateBuf[5];
extern u8 gBatLeveL[4];
extern u8 gSysStatus;
extern u8 gOutputStatus;
extern u16 gBatVoltArray[4][1];
extern u32 idata gLastChangeLevelTick[4];
extern u8 idata gIsFisrtChangeLevel[4];
void LED_ON(u8 led)
{
	switch(led)
	{
		case 1:
			P35=0;break;
		case 2:
			P34=0;break;
		case 3:
			P36=0;break;
		case 4:
			P37=0;break;
	}
}

void LED_OFF(u8 led)
{
	switch(led)
	{
		case 1:
			P35=1;break;
		case 2:
			P34=1;break;
		case 3:
			P36=1;break;
		case 4:
			P37=1;break;
	}	
}


u8 gCount= 0,ledOnFlag=0,isFirst = 0;

u32 ledDispalyTick = 0;
u8 gBatTempLevel[4];
void ledHandler(void)
{
	u8 i;
	
	/*
	//charging state
	if(getSysTick() & SHOW_CHARGING_TICK)
	{
		for(i=1;i<5;i++)
		{	
			if(gBatStateBuf[i] & 0x38)
				LED_ON(i);
		}
	}
	else
	{
		for(i=1;i<5;i++)
		{
			if((gBatStateBuf[i] & 0x38) && (gBatStateBuf[i] & CHARGE_STATE_FULL)==0)
				LED_OFF(i);
		}
	}
	*/
if(gSysStatus == SYS_CHARGING_STATE)
{
	do
	{
	if(getDiffTickFromNow(ledDispalyTick) > LED_DISPLAY_INTERVAL)
	{
		#ifdef LED_CHARGING_DISPLAY_SUPPORT
		if(isFirst == 0)
		{
			for(i=0;i<4;i++)
			{
				gBatTempLevel[i]= gBatLeveL[i];
				isFirst += gBatLeveL[i];
			}
			if(isFirst == 0)
				break;
			isFirst = 1;
		}
		if(getSysTick() & 0x10)
		{
			ledOnFlag = 1;
			if(gCount !=0)   //first skip
			{
				for(i=1;i<5;i++)
				{
					if(gBatLeveL[i - 1])
					{
						if(gBatTempLevel[i-1] >=gCount && (gBatStateBuf[i]&(CHARGE_STATE_ERROR | BAT_TYPE_ERROR)) ==0)
							LED_ON(i);
					}
				}
			}
		}
		else
		{
			if(ledOnFlag == 1)
			{
				gCount++;
				ledOnFlag = 0;				
			}

			for(i=1;i<5;i++)
			{
				if((gBatStateBuf[i] & (CHARGE_STATE_FULL | CHARGE_STATE_ERROR | BAT_TYPE_ERROR)) == 0)
					LED_OFF(i);		
			}
			
			if(gCount >4)
			{
				gCount =0;
				isFirst = 0;
				ledDispalyTick = getSysTick();
			}
		}
		#else
		if(getSysTick() & 0x10)
		{
			for(i=1;i<5;i++)
			{
				if((gBatStateBuf[i] & CHARGE_STATE_ALL)&& (gBatStateBuf[i]&(CHARGE_STATE_ERROR | BAT_TYPE_ERROR)) ==0)
					LED_ON(i);
			}
		}
		else
		{
			for(i=1;i<5;i++)
			{
				if((gBatStateBuf[i] & (CHARGE_STATE_FULL | CHARGE_STATE_ERROR | BAT_TYPE_ERROR)) == 0)
					LED_OFF(i);		
			}
		}
		#endif
		
	}
	}while(0);

	//error state

	if(getSysTick() & 0x08)
	{
		for(i=1;i<5;i++)
		{
			if(gBatStateBuf[i] & ((CHARGE_STATE_ERROR)|(BAT_TYPE_ERROR) ))
				LED_ON(i);	
		}
	}
	else
	{
		for(i=1;i<5;i++)
		{
			if(gBatStateBuf[i] & ((CHARGE_STATE_ERROR)|(BAT_TYPE_ERROR) ))
				LED_OFF(i);		
		}
	}
}
else
{
	if(gOutputStatus == OUTPUT_STATUS_NORMAL)
	{	
		if(gIsFisrtChangeLevel[0])
		{
			if(gIsFisrtChangeLevel[1])
			{
				if(getDiffTickFromNow(gLastChangeLevelTick[0]) > SHOW_CHARGING_TICK)
				{
					for(i=1;i<5;i++)
					{
						LED_OFF(i);
					}
					gIsFisrtChangeLevel[1] = 0;  //灭灯
					gLastChangeLevelTick[0] = getSysTick();
				}
			}
			else
			{
				if(getDiffTickFromNow(gLastChangeLevelTick[0]) > SHOW_OUTPUT_TICK)
				{
					for(i=1;i<=gBatLeveL[gCount];i++)
					{	
						LED_ON(i);
					}
					gIsFisrtChangeLevel[1] = 1;  //亮灯
					gLastChangeLevelTick[0] = getSysTick();
				}
			}
		}
		else
		{
			for(i=1;i<=gBatLeveL[gCount];i++)
			{	
				LED_ON(i);
			}
			gIsFisrtChangeLevel[0] =1;    //第一次进入
			gIsFisrtChangeLevel[1] = 1;  //亮灯
			gLastChangeLevelTick[0] = getSysTick();
		}

		
		#if 0
		if(getSysTick() & SHOW_OUTPUT_TICK)
		{
			for(i=1;i<=gBatLeveL[gCount];i++)
			{	
				LED_ON(i);
			}
		}
		else
		{
			for(i=1;i<5;i++)
			{
				LED_OFF(i);
			}
		}
		#endif
	}
}
	
	
}
