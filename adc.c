#include "nimh.h"
//u8 tempStr1[] = "temp1:";
//u8 tempStr2[] = "temp2:";

extern u16 gChargeCurrent;

u16 getAdcValue(u8 channel)
{
	
	u16 temp = 0xffff;
	u8 tempH,tempL;


	ADCCRL &= 0xF0;
	ADCCRL |= channel;

	ADCCRL |= (1<<6);
	
	while((ADCCRL&0x10) != 0x10)
	{
		temp--;
		if(temp == 0)
			break;
	}

	tempL = ADCDRL;	
	tempH = ADCDRH;
	

	
	temp = ((u16)tempH)<<8;
	temp += tempL;

	ADCCRL &= (~(1<<6));
	
	return temp;
}

u16 getAverage(u8 channel)
{
	u8 i;
	u16 temp,max,min,ret;


	temp = getAdcValue(channel);
	ret= temp;
	max =temp;
	min = temp;
	for(i=0;i<5;i++)
	{
		delay_us(100);
		 temp = getAdcValue(channel);
	 	if(temp > max)
	 	{
			max = temp;
	 	}

		 if(temp < min)
		{
			min = temp;
	 	}
	 	ret += temp;
	}
	

	return (ret - max - min)>>2;
}

u16 getBatTemp(u8 batNum)
{
	u16 tempT;



	if(batNum <3)
	{
		tempT = getAverage(CHANNEL_TEMP_1);
	}
	else
	{
		tempT = getAverage(CHANNEL_TEMP_2);
	}

	return tempT;
}

u16 getVbatAdc(u8 channel)
{
	u16 tempV;


	switch(channel)
	{
		case 1:
			channel = CHANNEL_VBAT_1;break;
		case 2:
			channel = CHANNEL_VBAT_2;break;
		case 3:
			channel = CHANNEL_VBAT_3;break;
		case 4:
			channel = CHANNEL_VBAT_4;break;
		default:
			break;
	}


	gChargeCurrent= getAverage(CHANNEL_20_RES);
	tempV = getAverage(channel);

//	sendStr(tempStr1);
//	send(gChargeCurrent);
//	sendStr(tempStr2);
//	send(temp2);

	if(tempV < gChargeCurrent)
	{
		gChargeCurrent = 0;
		return 0;
	}
	return (tempV-gChargeCurrent);
}
