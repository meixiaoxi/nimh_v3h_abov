#include "MC96F8316.h"
#include "nimh.h"
extern u8 gCount,ledOnFlag;

extern u32 ledDispalyTick;
extern u8 isFirst;
u32 idata gUpdateDebanceTick[4];
u8 gIsChargingBatPos=1;   //此刻正在充电电池的标记    
u8 gPreChargingBatPos = 0;
u32 shortTick=0;
u32 ChargingTimeTick = 0;
u16 gChargeCurrent;

u8 gSysStatus;
u8 gOutputStatus;

u8  isFromOutput = 0;

//type_error charge_error  pre  fast sup  trick  charging(on_off)  is_detect(电池检测) bat_state(valid)
//the first byte is a dummy						
u8 gBatStateBuf[5] = {0x0,0x00,0x00,0x00,0x00};

u16 gBatVoltArray[4][1]={{0},{0},{0},{0}};

u16 gTempVolt;

u8 gChargeSkipCount[] = {0,0,0,0};    //控制PWM周期

u8 gBatNumNow = 0;

		// 1/2/3/4号电池
u8 gBatNowBuf[5]={0,0,0,0,0};  //存放充电器上电池的标号
u8 gBatLeveL[] = {0,0,0,0};
u16 preVoltData[4] ={0,0,0,0};

u8 gAdcCount[4] = {0,0,0,0};

u16 gLowCurrentCount=0;

u8 skipCount;
u16 isPwmOn = 0;

u8 dropCount[4] = {0,0,0,0};	
u8 fitCount[4] = {0,0,0,0};

u32 gChargingTimeTick[4] ={0,0,0,0};


void updateBatLevel(u16 tempV,u8 batNum)
{
	u8 level;


	if(gBatLeveL[batNum-1] == 0)
	{
		/*
		if(tempV > BAT_LEVEL_43_IDLE)
			gBatLeveL[batNum-1] = 4;
		else if(tempV > BAT_LEVEL_32_IDLE)
			gBatLeveL[batNum-1] = 3;
		else if(tempV > BAT_LEVEL_21_IDLE)
			gBatLeveL[batNum-1] = 2;
		else
			gBatLeveL[batNum-1] = 1;					
		
		gUpdateDebanceTick[batNum-1] = getSysTick();
		*/
		if(isFirst == 0)
		{
			ledDispalyTick = getSysTick()-LED_DISPLAY_INTERVAL-1;
		}
		
	}

	if(gSysStatus == SYS_CHARGING_STATE)
	{
		if(tempV > BAT_LEVEL_34_CHARGING)
			level = 4;
		else if(tempV > BAT_LEVEL_23_CHARGING)
			level = 3;
		else if(tempV > BAT_LEVEL_12_CHARGING)
			level =2;
		else
			level =1;
		if(gBatLeveL[batNum-1])
		{
			if(level > gBatLeveL[batNum-1])
				gBatLeveL[batNum-1] = level;
		}
		else
		{
			gBatLeveL[batNum-1] = level;
		}
	}
	else
	{
		if(tempV > BAT_LEVEL_43_OUTPUT)
			level = 4;
		else if(tempV > BAT_LEVEL_32_OUTPUT)
			level = 3;
		else if(tempV > BAT_LEVEL_21_OUTPUT)
			level =2;
		else
			level =1;

		if(gBatLeveL[batNum-1])
		{
			if(level < gBatLeveL[batNum-1])
				gBatLeveL[batNum-1] = level;
		}
		else
			gBatLeveL[batNum-1] = level;
	}
	
	gUpdateDebanceTick[batNum-1] = getSysTick();
}


void outputHandler()
{
	u8 cur_detect_pos;
do
{
	if(gOutputStatus != OUTPUT_STATUS_STOP)
	{
		if(/*gOutputStatus == OUTPUT_STATUS_WAIT  ||*/ getDiffTickFromNow(ChargingTimeTick) > OUTPUT_CHECK_INTERVAL)
		{
			ChargingTimeTick = getSysTick();
			#if 0
			preVoltData[0] = getAverage(7);
			if(preVoltData[0] <85)
			{
				if(gOutputStatus  == OUTPUT_STATUS_NORMAL)
				{
					gLowCurrentCount++;
					if(gLowCurrentCount>=40)
					{
						gOutputStatus = OUTPUT_STATUS_STOP;
						DISABLE_BOOST();
						LED_OFF(1),LED_OFF(2),LED_OFF(3),LED_OFF(4);
						break;
					}
				}
			}
			#endif
			preVoltData[0] = getVbatAdc(1);
			if(gOutputStatus == OUTPUT_STATUS_WAIT)
			{
				gBatVoltArray[1][0] = 0xFFFF;
				gCount = 1;
				for(cur_detect_pos = 1; cur_detect_pos < 4; cur_detect_pos++)
				{
					preVoltData[cur_detect_pos] = getVbatAdc(cur_detect_pos+1);

					if(cur_detect_pos == 2 || cur_detect_pos == 3)
						preVoltData[cur_detect_pos] = preVoltData[cur_detect_pos]/3;
					
					gBatVoltArray[0][0] = preVoltData[cur_detect_pos-1] -preVoltData[cur_detect_pos];
					if(gBatVoltArray[0][0] < gBatVoltArray[1][0])
					{
						gCount = cur_detect_pos;
						gBatVoltArray[1][0] = gBatVoltArray[0][0];
					}
					if(preVoltData[cur_detect_pos] > preVoltData[cur_detect_pos-1] ||( gBatVoltArray[0][0]<MIN_VBAT_OUTPUT_IDLE))
						break;	
				}
				if(cur_detect_pos == 4)
				{
					gOutputStatus = OUTPUT_STATUS_NORMAL;
					ENABLE_BOOST();
					updateBatLevel(gBatVoltArray[1][0],gCount+1);
				}
				else
				{
					gOutputStatus = OUTPUT_STATUS_WAIT;
					LED_OFF(1),LED_OFF(2),LED_OFF(3),LED_OFF(4);
					DISABLE_BOOST();
				}				
			}
			else
			{
				for(cur_detect_pos = 1; cur_detect_pos < 4; cur_detect_pos++)
				{
					preVoltData[cur_detect_pos] = getVbatAdc(cur_detect_pos+1);

					if(cur_detect_pos == 2 || cur_detect_pos == 3)
						preVoltData[cur_detect_pos] = preVoltData[cur_detect_pos]/2;
					
					gBatVoltArray[0][0] = preVoltData[cur_detect_pos-1] -preVoltData[cur_detect_pos];
					
					if(cur_detect_pos == gCount)
						gBatVoltArray[1][0] = gBatVoltArray[0][0];
					
					if(preVoltData[cur_detect_pos] > preVoltData[cur_detect_pos-1] ||( gBatVoltArray[0][0] < MIN_VBAT_OUPUT))
						break;	
				}
				if(cur_detect_pos != 4)
				{
					gOutputStatus = OUTPUT_STATUS_WAIT;
					LED_OFF(1),LED_OFF(2),LED_OFF(3),LED_OFF(4);
					DISABLE_BOOST();
				}
				else if(getDiffTickFromNow(gUpdateDebanceTick[gCount]) > MIN_BAT_LEVEL_UPDATE_INTERVAL)
					updateBatLevel(gBatVoltArray[1][0],gCount+1);
			}
			#if 0
			if(preVoltData[0] < MIN_VBAT_CHANNEL_1_IDLE)
			{
				DISABLE_BOOST();
				LED_OFF(1),LED_OFF(2),LED_OFF(3),LED_OFF(4);
				gOutputStatus = OUTPUT_STATUS_WAIT;
				break;
			}
			#endif
			
		}
	}
}while(0);
}

void PreCharge(u8 batNum)
{
	u16 tempT;

		gChargingTimeTick[batNum-1]++;
		
		if(gChargingTimeTick[batNum-1] > BAT_CHARGING_PRE_MAX_COUNT)
		{
			gBatStateBuf[batNum] &= ~CHARGE_STATE_PRE;
			gBatStateBuf[batNum] |=BAT_TYPE_ERROR;
		}
		else
		{ 
			gBatVoltArray[batNum-1][0] = getVbatAdc(batNum);
			if(gBatVoltArray[batNum-1][0] >= CHARGING_PRE_END_VOLT )
			{				
				gChargingTimeTick[batNum-1] = 0;
				gBatStateBuf[batNum] &= ~CHARGE_STATE_PRE;
				gBatStateBuf[batNum] |= CHARGE_STATE_FAST;
			}
			else
			{
				tempT = getBatTemp(batNum);
				if(tempT < ADC_TEMP_MAX || tempT > ADC_TEMP_MIN)
				{
					gBatStateBuf[batNum] |= CHARGE_STATE_ERROR;
					gChargingTimeTick[batNum-1] = 0;
				}
			}
				
		}
}


void FastCharge(u8 batNum)
{
	u16 tempV,tempT,tempAvg;

	gChargingTimeTick[batNum-1]++;
	tempT = getBatTemp(batNum);

	if(getDiffTickFromNow(gUpdateDebanceTick[batNum-1]) > MIN_BAT_LEVEL_UPDATE_INTERVAL)
		updateBatLevel(tempV,batNum);

	if(gChargingTimeTick[batNum-1] > BAT_START_DV_COUNT)  //hod-off time, in this period, we do NOT detect -dv
	{
		//gBatVoltTemp[batNum-1][testData[batNum-1]] = tempV;
		//testData[batNum-1]++;
		if(gAdcCount[batNum-1] > 23 || preVoltData[batNum-1] > BAT_VOLT_CHANGE_FASTER)
		{
			gAdcCount[batNum-1]  = 0;
			tempV = getVbatAdc(batNum);
			if(preVoltData[batNum-1])
			{
				tempV = ((preVoltData[batNum-1]<<2) + tempV)/5;
				preVoltData[batNum-1] = tempV;
			}
			else
			{
				preVoltData[batNum-1] = tempV;
			}
		}
		else
		{
			gAdcCount[batNum-1]++;
			if(preVoltData[batNum-1] != 0)
				tempV = preVoltData[batNum-1];
			else
			{
				tempV = getVbatAdc(batNum);
				preVoltData[batNum-1] = tempV;
			}
		}
		
		/*
		tempV = ((gBatVoltArray[batNum-1][0]<<2)+tempV)/5;
		send(tempV);
		*/
		
		//if(getDiffTickFromNow(gChargingIntervalTick[batNum-1]) > BAT_DV_CHECK_INTERVAL)
		{
			/*
			gChargingIntervalTick[batNum-1] = getSysTick();
			if(gChargingIntervalTick[batNum-1] == 0)
			{
				GIE = 0;
				shortTick = 1;
				GIE = 1;
				gChargingIntervalTick[batNum-1] = 1;
			}
			*/
			
			if(tempV >= CHARGING_FAST_MAX_VOLT || gChargingTimeTick[batNum-1] > BAT_CHARGING_FAST_MAX_COUNT || tempT < ADC_TEMP_MAX || tempT > ADC_TEMP_MIN)
			{
				if((tempT < ADC_TEMP_MAX  && tempV <CHARGING_FAST_TEMP_END_VOLT) || tempT > ADC_TEMP_MIN)   //过温
				{
					//电压不足
					gBatStateBuf[batNum] |= CHARGE_STATE_ERROR;

					preVoltData[batNum-1] = 0;
					return;
				}
				fitCount[batNum-1]++;
				if(fitCount[batNum-1] > 3)
				{
					gBatStateBuf[batNum] |= CHARGE_STATE_FULL;
					 //timer or maxVolt
					gBatStateBuf[batNum] &= ~CHARGE_STATE_FAST;
					gBatStateBuf[batNum] |= CHARGE_STATE_TRICK;
					gBatStateBuf[batNum] |= CHARGE_STATE_FULL;

					gChargingTimeTick[batNum-1] = 0;
				}
					return;
			}
			fitCount[batNum-1] = 0;
		//	if(testData[batNum-1] == 4)
			{
				/*
				testData[batNum-1] = 0;
				tempAvg = getTestAvg(batNum);
				if(preVoltData[batNum-1])
				{
					tempAvg = (preVoltData[batNum-1]<<2 + tempAvg)/5;
				}
				else
				{
					preVoltData[batNum-1] = tempAvg;
				}
				*/
			if(tempV > gBatVoltArray[batNum-1][0])
			{
				gBatVoltArray[batNum-1][0] = tempV;
				dropCount[batNum-1] = 0;
			}
			else
			{
				if((gBatVoltArray[batNum-1][0] - tempV) > ADC_DV_VOLT)
				{
					if(tempV < DV_ENABLE_MIN_VOLT)
						return;
					dropCount[batNum-1]++;
					if(dropCount[batNum-1] >3)
					{
					gBatStateBuf[batNum] &= ~CHARGE_STATE_FAST;
					gBatStateBuf[batNum] |= CHARGE_STATE_TRICK;
					gBatStateBuf[batNum] |= CHARGE_STATE_FULL;
					gChargingTimeTick[batNum-1] = 0;
					}
				}
			}
			}
		}
	}
	else
	{
		tempV = getVbatAdc(batNum);
		gBatVoltArray[batNum-1][0] = tempV;

		if(tempV > BAT_INITIAL_VOLT_FULL && gChargingTimeTick[batNum-1] > BAT_INITIAL_FULL_CHECK_COUNT)
		{
			gBatStateBuf[batNum] &= ~CHARGE_STATE_FAST;
			gBatStateBuf[batNum] |= CHARGE_STATE_TRICK;
			gBatStateBuf[batNum] |= CHARGE_STATE_FULL;
			gChargingTimeTick[batNum-1] = 0;
		}
		
		if(tempT < ADC_TEMP_MAX || tempT > ADC_TEMP_MIN )
		{
			gBatStateBuf[batNum] |= CHARGE_STATE_ERROR;
		}
	}

}

/*
void SupCharge(u8 batLabel)
{

}
*/
void TrickCharge(u8 batNum)
{
	if(gBatStateBuf[batNum] & CHARGE_STATE_FULL)
	{
	}
	else
	{

		gBatVoltArray[batNum-1][0] = getBatTemp(batNum);

		if(gBatVoltArray[batNum-1][0]>ADC_TEMP_MAX && gBatVoltArray[batNum-1][0] <ADC_TEMP_MIN)
		{
			gBatStateBuf[batNum] &= ~CHARGE_STATE_TRICK;
			gBatStateBuf[batNum] |= CHARGE_STATE_FAST;
			//gChargingTimeTick[batNum-1] = 0;
		}
	}
}

void PwmControl(unsigned char status)
{
	if(status == PWM_OFF)
	{	
		isPwmOn =0;

	switch(gBatNowBuf[gIsChargingBatPos])
	{
		case 1:
			P30 =0;
			break;
		case 2:
			P31 =0;
			break;
		case 3:
			P33 =0;
			break;
		case 4:
			P23 =0;
			break;
		default:
			break;
	}
	}
	else
	{
	switch(gBatNowBuf[gIsChargingBatPos])
	{
		case 1:
			P30 =1;
			break;
		case 2:
			P31 =1;
			break;
		case 3:
			P33 =1;
			break;
		case 4:
			P23 =1;
			break;
		default:
			break;
	}		
	}
}

void removeBat(u8 toChangeBatPos)
{
	u8 i;

	ChargingTimeTick = 0;
	gAdcCount[gBatNowBuf[toChangeBatPos] -1] = 0;
	gBatLeveL[gBatNowBuf[toChangeBatPos] -1] = 0;
	gChargeSkipCount[gBatNowBuf[toChangeBatPos] -1 ] = 0;
	gChargingTimeTick[gBatNowBuf[toChangeBatPos] - 1] = 0;
	//gChargingIntervalTick[gBatNowBuf[toChangeBatPos]-1] = 0;
	gBatStateBuf[gBatNowBuf[toChangeBatPos]] =0;
	dropCount[gBatNowBuf[toChangeBatPos] - 1] = 0;
	fitCount[gBatNowBuf[toChangeBatPos] - 1] = 0;
	preVoltData[gBatNowBuf[toChangeBatPos]-1] = 0;
	LED_OFF(gBatNowBuf[toChangeBatPos]);
	//PB &= 0xF0;   //close current pwm channel
	PwmControl(PWM_OFF);

	for(i=toChangeBatPos;i<gBatNumNow;i++)
	{
		gBatNowBuf[i] = gBatNowBuf[i+1];
	}
	gBatNowBuf[gBatNumNow] = 0;
	gBatNumNow--;
	gIsChargingBatPos = 1;
	gPreChargingBatPos = 0;
}

void removeAllBat()
{
	u8 i;

	ChargingTimeTick = 0;
	for(i=0;i<4;i++)
	{
		gBatLeveL[i] = 0;
		gChargeSkipCount[i] = 0;
		gChargingTimeTick[i] = 0;
		//gChargingIntervalTick[gBatNowBuf[toChangeBatPos]-1] = 0;
		preVoltData[i] = 0;
		LED_OFF(i+1);
		gBatStateBuf[i] =0;
		gBatNowBuf[i]=0;
		dropCount[i]=0;
		fitCount[i] = 0;
		gAdcCount[i] = 0;
	}
	gBatNowBuf[4]=0;
	gBatStateBuf[4] =0;

	//PWM
	P30 =0;
	P31 =0;
	P33 =0;
	P23 =0;

	isPwmOn =0; 
	
	gBatNumNow=0;
	gIsChargingBatPos = 1;
	gPreChargingBatPos = 0;
}

void chargeHandler(void)
{
	u16 tempV;
	u16 tempT; 
	//close all pwm
	//PB &= 0xF0;

	if(isFromOutput == 1)
	{
		if(getDiffTickFromNow(ChargingTimeTick) > BAT_CHARGING_DELAY_FROM_OUTPUT)
		{
			isFromOutput = 0;
			ChargingTimeTick = 0;
		}
		else
			return;
	}
	if(gBatNumNow ==0)
		return;

	if(ChargingTimeTick == 0)   
	{
		switch(gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & 0x38)	
		{
			case CHARGE_STATE_FAST:
					skipCount = FAST_SKIP_COUNT; break; //BatFastCharge(ticknow%4);break;
		//	case CHARGE_STATE_SUP:
		//			skipCount = SUP_SKIP_COUNT;break;// BatSupCharge(ticknow%4);break;
			case CHARGE_STATE_PRE:
					skipCount = PRE_SKIP_COUNT;break;//BatPreCharge(ticknow%4);break;
			case CHARGE_STATE_TRICK:
					skipCount = TRI_SKIP_COUNT;break;//BatTrickCharge(ticknow%4);break;
			default:
					break;
		}

		ChargingTimeTick = getSysTick();
		if(ChargingTimeTick == 0)
		{
			EA =0 ;
			shortTick = 1;
			EA =1;
			ChargingTimeTick =1;
		}
		if(gIsChargingBatPos !=0)    //0 pos is a dummy
		{
			//电池类型错误    充电错误
			if(gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & ((BAT_TYPE_ERROR) |(CHARGE_STATE_ERROR)))
			{
				tempT = getBatTemp(gBatNowBuf[gIsChargingBatPos]);

				if(tempT> ADC_TEMP_MAX && tempT < ADC_TEMP_MIN)
					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] &= (~CHARGE_STATE_ERROR);
				tempV = getVbatAdc(gBatNowBuf[gIsChargingBatPos]);
				if(tempV < BAT_MIN_VOLT_OPEN)
				{
					ChargingTimeTick = 0;
					removeBat(gIsChargingBatPos);
				}

				gBatVoltArray[gBatNowBuf[gIsChargingBatPos]-1][0] = tempV;
				return;	
			}
	
			if(gChargeSkipCount[gBatNowBuf[gIsChargingBatPos]-1] >=skipCount)   //ok, it's pulse now
			{
				/*
				PB &= 0xF0;   //close all pwm
				PB |= (1<<(gBatNowBuf[gIsChargingBatPos]-1));
				*/
				PwmControl(PWM_ON);
				gChargeSkipCount[gBatNowBuf[gIsChargingBatPos]-1] = 0;
				isPwmOn =1;
			}
			else
				gChargeSkipCount[gBatNowBuf[gIsChargingBatPos] - 1]++;
		}
	}
	else
	{
		 if(gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & BAT_DETECT_BIT)   //电池检测
		{
			if(getDiffTickFromNow(ChargingTimeTick)>BAT_CHARGING_PWMOFF_DETECT_TIME)
			{
				if(gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & BAT_TYPE_ERROR)
					return;
				tempV = getVbatAdc(gBatNowBuf[gIsChargingBatPos]);
				if(isPwmOn)
				{
					if(gBatNowBuf[gIsChargingBatPos] == 4)
						isPwmOn = BAT_MAX_VOLT_CLOSE_CHANNEL_4;
					else
						isPwmOn = BAT_MAX_VOLT_CLOSE;
					
					 if(tempV>isPwmOn || tempV < BAT_MIN_VOLT_OPEN || gChargeCurrent <3  )
					{
						gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] &= ~(BAT_DETECT_BIT |CHARGE_STATE_ALL);
						gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= BAT_TYPE_ERROR;
						gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= HAS_BATTERY;
						//PB &= 0xF0;   //close current pwm channel
						PwmControl(PWM_OFF);
						ChargingTimeTick = 0;
						return;
					}
				}
				//PB &= 0xF0;
				PwmControl(PWM_OFF);
				
				if(tempV<BAT_MIN_VOLT_OPEN)   //电池被拔出
				{
					ChargingTimeTick = 0;
					removeBat(gIsChargingBatPos);
				}
				//ENABLE_ADC_DELAY_DETECT
//				else if(tempV>BAT_MAX_VOLT_CLOSE || gChargeCurrent <3  /* || (gChargeCurrent<<3) <= ( tempV-gBatVoltArray[gBatNowBuf[gIsChargingBatPos]-1][0])*/)  //
//				{
//					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] &= ~(BAT_DETECT_BIT |CHARGE_STATE_ALL);
//					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= BAT_TYPE_ERROR;
//					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= HAS_BATTERY;
//					//PB &= 0xF0;   //close current pwm channel
//					PwmControl(PWM_OFF);
//					ChargingTimeTick = 0;
//				}
//				else   //电池检测ok
				else if(getDiffTickFromNow(ChargingTimeTick) > BAT_CHARGING_DETECT_TIME)
				{
//					delay_ms(100);
					tempV = getVbatAdc(gBatNowBuf[gIsChargingBatPos]);
					//gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] &= ~ BAT_DETECT_BIT;
					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] = 0;
					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= HAS_BATTERY;
					tempT = getBatTemp(gBatNowBuf[gIsChargingBatPos]);
				

				//	if(tempT < ADC_TEMP_MAX || tempT > ADC_TEMP_MIN)
				//	{
				//		gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= CHARGE_STATE_TRICK;
				//	}
				//	else
					{
						/*
						if(gBatVoltArray[gBatNowBuf[gIsChargingBatPos]-1][0] < CHARGING_PRE_END_VOLT)
							gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= CHARGE_STATE_PRE;
						else if(gBatVoltArray[gBatNowBuf[gIsChargingBatPos]-1][0] < CHARGING_FAST_END_VOLT)
							gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= CHARGE_STATE_FAST;
						else
							gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= (CHARGE_STATE_TRICK | CHARGE_STATE_FULL);
						*/
						if(tempV< CHARGING_PRE_END_VOLT)
							gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= CHARGE_STATE_PRE;
						else
							gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= CHARGE_STATE_FAST;

						if(tempT < ADC_TEMP_MAX || tempT > ADC_TEMP_MIN)
						{
							gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= CHARGE_STATE_ERROR;
						}

						gChargingTimeTick[gBatNowBuf[gIsChargingBatPos]-1] = 0;

						updateBatLevel(tempV,gBatNowBuf[gIsChargingBatPos]);
					}
						
				}
  					
  			}
		}
		 else if(getDiffTickFromNow(ChargingTimeTick)  > BAT_CHARGING_PULSE_TIME)   //change to next channel
		{	
			//PB &= 0xF0;   //close current pwm channel
			PwmControl(PWM_OFF);

			//	if((gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & 0x38) == CHARGE_STATE_FAST)
			//		FastCharge(gBatNowBuf[gIsChargingBatPos]);
			//ENABLE_ADC_DELAY_DETECT

			if((gBatStateBuf[gBatNowBuf[gPreChargingBatPos]] & ((BAT_TYPE_ERROR) |(CHARGE_STATE_ERROR))) || gPreChargingBatPos ==0)
			{
				ChargingTimeTick = 0;
				gPreChargingBatPos = gIsChargingBatPos;
				if(gIsChargingBatPos >= gBatNumNow)
				{
					if(gBatNumNow%2)
					{
						gIsChargingBatPos =0;
					}
					else
						gIsChargingBatPos =1;
				}
				else
					gIsChargingBatPos++;				
			}
			else //if(gPreChargingBatPos !=0)
			{
				if(getDiffTickFromNow(ChargingTimeTick) > BAT_CHARGING_PULSE_DELAY_TIME)
		 		{
					switch(gBatStateBuf[gBatNowBuf[gPreChargingBatPos]] & 0x38)
					{
						case CHARGE_STATE_FAST:
								FastCharge(gBatNowBuf[gPreChargingBatPos]);break;
					//	case CHARGE_STATE_SUP:
					//			 SupCharge(gBatNowBuf[gIsChargingBatPos]);break;
						case CHARGE_STATE_PRE:
								PreCharge(gBatNowBuf[gPreChargingBatPos]);break;
						case CHARGE_STATE_TRICK:
								TrickCharge(gBatNowBuf[gPreChargingBatPos]);break;
						default:
							break;
					}

					ChargingTimeTick = 0;
					gPreChargingBatPos = gIsChargingBatPos;
					if(gIsChargingBatPos >= gBatNumNow)
					{
						if(gBatNumNow%2)
						{
							gIsChargingBatPos =0;
						}
						else
							gIsChargingBatPos =1;
					}
					else
						gIsChargingBatPos++;					
					
				}
			}		  
		}
		else if(/*gIsChargingBatPos !=0 && */(getDiffTickFromNow(ChargingTimeTick) > BAT_CHARGING_TEST_TIME))
		{
			if(gIsChargingBatPos != 0) //note here
			{
			tempV = getVbatAdc(gBatNowBuf[gIsChargingBatPos]);

			//if((gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & 0x38) == CHARGE_STATE_TRICK)
			//{
			//	if(getDiffTickFromNow(ChargingTimeTick) > BAT_CHARGING_TRICK_TIME)
			//		PB &=0xF0;
			//}
			/*
			if(gChargeCurrent<44)    // <1800mA
			{
				LED_OFF(3);LED_OFF(4);LED_ON(1);
			}
			else if(gChargeCurrent<54)  //<2200mA
			{
				LED_OFF(1);LED_OFF(3);LED_ON(4);
			}
			else
			{
				LED_OFF(1);LED_OFF(4);LED_ON(3);
			}
			*/
			if(tempV < BAT_MIN_VOLT_OPEN || (isPwmOn && gChargeCurrent <3))
			{
					ChargingTimeTick = 0;
					removeBat(gIsChargingBatPos);
			}
			}
			//ENABLE_ADC_DELAY_DETECT
		}
	}
}

void StatusCheck()
{
		if(gSysStatus != (GET_SYS_STATUS()))
		{
			if(gSysStatus == SYS_CHARGING_STATE)
			{
				delay_ms(2);
				if(gSysStatus != (GET_SYS_STATUS()))
				{
				gLowCurrentCount = 0;
				gSysStatus = SYS_DISCHARGE_STATE;
				CHANGE_TO_OUTPUT();
				gOutputStatus = OUTPUT_STATUS_WAIT;				
				//remove all bat
				removeAllBat();
				ChargingTimeTick = getSysTick();
				gCount= 0;
				ledOnFlag=0;
				ledDispalyTick = 0;
				gPreChargingBatPos =0;
				isFirst = 0;
				isFromOutput = 0;
				}
			}
			else
			{
				gCount = 0;
				DISABLE_BOOST();
				gSysStatus = SYS_CHARGING_STATE;
				CHANGE_TO_INPUT();
				isFromOutput = 1;
				removeAllBat();
				ChargingTimeTick = getSysTick();
			}
		}	
}
void InitConfig()
{					 
					  //7     6      5      4             3               2                 1             0
       				  //-     -      -      -      Vin5V_DET   V2+_H_CTL     NTC2     iout_cc         
    P0IO    = 0xF0;         // out     out    out     out        input              input           input       input                 (0:input   1:output)
    P0OD    = 0x00;        // -      pp     pp      pp            PP                PP               pp            pp                    (0:push-pull   1:open-drain)
    P0PU    = 0x70;         // -      on      on       on           off                off             off           off                  (0:disable      1:enable)               
    P0        = 0x00;	        // -      -       -         -              0              0                0               0
    P03DB   = 0x00;       // 0       0      0       0               0              0                 0              0
    P0FSR   = 0x13;       // 0      0      0       1               0              0                   1            1

                                    //-     V4+_DET    NTC1   V2+_DET   V1+_DET   GND_ALL   V3+_DET   CHG_DISCHG
    P1IO    = 0x80;         // out      input        input        input         input          input           input             inut
    P1OD    = 0x00;        // pp        PP           PP           pp             PP             PP             PP                 pp
    P1PU    = 0x80;        // on        off          off           off            off             off            off                 off
    P1	  = 0x00;        // 00000000
    P12DB   = 0x00;       // 00000000
    P1FSRH  = 0x2A;      // 00       10            10            10
     P1FSRL  = 0x54;	 //                                                      010              10            10                 0 
    
                                    //-    -  boost_en     v1+_h_ctl   pwm4   -         -      -
    P2IO    = 0x6F;         //-   out     out               input         out    out     out     out
    P2OD    = 0x00;         // -   PP      PP              PP             PP      PP      pp     pp
    P2PU    = 0x47;         // -    on     off              off             off     on      on     on
    P2	  = 0x00;		    // -      -      1      1      -      -      -      -
    P2FSR   = 0x00;	   //             00000000

                                     //led4    led3    led1    led2    pwm3    RST    pwm2   pwm1
    P3IO    = 0xFF;         // out       out      out       out      out       out       out       out
    P3OD    = 0x00;        // PP         PP      PP         PP       PP         PP       PP       PP
    P3PU    = 0x00;         // off       off      off        off       off       off       off       off
    P3	   = 0xF0;	//00000000
    P3FSR   = 0x00;		  // 0        0         0          0         0         0          0        0	
}

void main()
{
	u8 cur_detect_pos=1,tempN;
	OSCCR = 0x20;		// internal OSC 8MHz
	BITCR = 0x4E;		// BIT 16.384ms

	delay_ms(16);

	LVRCR  = 0x02;                      // builtin reset 2.00V set, LVRCR.0=0 enable !!!

	InitConfig();

	WDTDR = 0xFF;
	ClrWdt();
	
	//timer0   as system tick
	T0DR = 0xFF;
	T0CR = 0x8B;

	//ADC
	ADCCRH = 0x07;
	ADCCRL  = 0x90;

	//interrupt
	IE = 0x00;
	IE1 = 0x00;
	IE2 = 0x02;    //enable timer0 overflow 
	IE3 = 0x00;
	EIFLAG0 = 0;
	EIFLAG1 = 0;
	IIFLAG = 0;

	IE |= (1<<7);    //global interrupt

	isFromOutput = 0;

	gSysStatus =  GET_SYS_STATUS();
	if(gSysStatus == SYS_DISCHARGE_STATE)
	{
		gOutputStatus = OUTPUT_STATUS_WAIT;
		CHANGE_TO_OUTPUT();
	}
		LED_ON(1);
		delay_ms(100);
		LED_OFF(1);
		delay_ms(100);
		LED_ON(1);
		delay_ms(100);
		LED_OFF(1);

	
	while(1)
	{

		StatusCheck();

		if(gSysStatus == SYS_CHARGING_STATE)
		{
			if(gBatStateBuf[cur_detect_pos] & HAS_BATTERY)
			{
				
				if(getVbatAdc(cur_detect_pos) < BAT_MIN_VOLT_OPEN)
				{
					tempN = 1;
					do{
						if(gBatNowBuf[tempN] == cur_detect_pos)
						{
							removeBat(tempN);
							break;
						}
						tempN++;
					}while(tempN < 5);
				}
			}
			else
			{
				if((gBatStateBuf[cur_detect_pos] & BAT_DETECT_BIT) == 0)
				{	
					gBatVoltArray[cur_detect_pos-1][0]=  getVbatAdc(cur_detect_pos);	//gBatVoltArray[cur_detect_pos-1][0]= getVbatAdc(cur_detect_pos);
				
					if(gBatVoltArray[cur_detect_pos-1][0] >= BAT_MIN_VOLT_OPEN)
					{
						if(gBatVoltArray[cur_detect_pos-1][0] > BAT_MAX_VOLT_OPEN)
							gBatStateBuf[cur_detect_pos] |= (BAT_TYPE_ERROR|HAS_BATTERY);
						else
							gBatStateBuf[cur_detect_pos] |= (CHARGE_STATE_FAST|BAT_DETECT_BIT);
	
							gBatNumNow++;	
							gBatNowBuf[gBatNumNow] = cur_detect_pos;
					}
				}
			}

			chargeHandler();
		
			cur_detect_pos++;
			if(cur_detect_pos > 4)
				cur_detect_pos=1;
		}	
		else    //output handler
			outputHandler();

		ledHandler();
		
		ClrWdt();

	}
	
	
}



void T0_Int_Handler(void) interrupt 13
{
	shortTick++;
}
