#include "nimh.h"

extern u8 gBatStateBuf[5];
extern u8 gBatLeveL[4];
extern u8 gSysStatus;
extern u8 gOutputStatus;
extern u16 gBatVoltArray[4][1];
extern u32 idata gLastChangeLevelTick[4];
extern u8 idata gIsFisrtChangeLevel[4];
extern u32 shortTick;
void LED_ON(u8 led)
{
	switch(led)
	{
		case 1:
			P35=0;break;
		case 2:
			P34=0;break;
		case 3:
			#ifdef EVT_BOARD
			P36=0;break;
			#else
			P00=0;break;
			#endif
		case 4:
			P37=0;break;
		default:
			break;
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
			#ifdef EVT_BOARD
			P36=1;break;
			#else
			P00=1;break;
			#endif
		case 4:
			P37=1;break;
		default:
			break;
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
		#endif
		
	}
	}while(0);
	
	#ifndef LED_CHARGING_DISPLAY_SUPPORT
	if(getSysTick() & SHOW_CHARGING_TICK)
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
		#ifdef LED_OUTPUT_DISPLAY_SUPPORT
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
		#else
		/*********
		gIsFisrtChangeLevel[0]用于轮流闪开始的标志
		gIsFisrtChangeLevel[1]用于放电状态下初始四个灯亮的结束标志
		gIsFisrtChangeLevel[2]用于表示当前处于亮灯状态的灯
		gIsFisrtChangeLevel[3]用于轮流亮时亮/灭的标志

		**********/
		if(gIsFisrtChangeLevel[1])
		{
			if(gIsFisrtChangeLevel[0])
			{
				if(gIsFisrtChangeLevel[3])  //亮
				{
					if(getDiffTickFromNow(gLastChangeLevelTick[0]) > LED_DISPLAY_ON)
					{
						LED_OFF(gIsFisrtChangeLevel[2]);
						if(gIsFisrtChangeLevel[2] ==4)
							gIsFisrtChangeLevel[2] =1;
						else
							gIsFisrtChangeLevel[2]++;
						gLastChangeLevelTick[0] = getSysTick();
						gIsFisrtChangeLevel[3] = 0;
					}
				}
				else  //灭
				{
					if(getDiffTickFromNow(gLastChangeLevelTick[0]) > LED_DISPLAY_OFF)
					{
						LED_ON(gIsFisrtChangeLevel[2]);
						gLastChangeLevelTick[0] = getSysTick();
						gIsFisrtChangeLevel[3] = 1;
					}
				}
			}
		      else //第一次
		      {
				gIsFisrtChangeLevel[0] = 1;
				gIsFisrtChangeLevel[2] =1;
				gIsFisrtChangeLevel[3] = 1;
				LED_ON(1);
				gLastChangeLevelTick[0] = getSysTick();
			}
		}
		else
		{
			if(gLastChangeLevelTick[0])
			{
				if(getDiffTickFromNow(gLastChangeLevelTick[0]) > LED_INITIAL_DISPLAY)
				{
					for(i=1;i<5;i++)
						LED_OFF(i);
				}
				if(getDiffTickFromNow(gLastChangeLevelTick[0]) > LED_INITIAL_DISPLAY_END)
				{
					gIsFisrtChangeLevel[1]=1;
				}
			}
			else
			{
				for(i=1;i<5;i++)
					LED_ON(i);
				gLastChangeLevelTick[0] = getSysTick();
				if(gLastChangeLevelTick[0] == 0)
				{
					EA = 0;
					shortTick = 1;
					EA =1;
					gLastChangeLevelTick[0] = 1;
				}
			}
		}
	#endif
	}
}
	
	
}
