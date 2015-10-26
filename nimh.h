#ifndef __NIMH_H__
#define __NIMH_H__

#include "MC96F8316.h"

typedef  unsigned char		u8;
typedef	 unsigned short 		u16;		
typedef 	 unsigned long		u32;

#define		cli()		do{IE &= ~0x80;}while(0)
#define		sei()		do{IE |=  0x80;}while(0)
#define		NOP()		_nop_()
#define 		ClrWdt()		WDTCR = 0xE0

#define 		GET_SYS_STATUS()	P1&0x01
#define		CHANGE_TO_OUTPUT()	(P0IO |= (1<<2),P02 = 0,P2IO |= (1<<4),P24 = 0)
#define		CHANGE_TO_INPUT()		(P0IO &= ~(1<<2),P2IO &=~(1<<4))
#define PWM_ON	1
#define PWM_OFF	 0

#define SYS_CHARGING_STATE	0x00
#define SYS_DISCHARGE_STATE	0x01

#define	BAT_MAX_LABEL 		4

#define	BAT_ADD			0
#define	BAT_REMOVE		1


#define HAS_BATTERY		0x01   

#define CHARGE_STATE_ALL		0x3C

#define BAT_TYPE_ERROR						(1<<7)
#define CHARGE_STATE_ERROR				(1<<6)
#define CHARGE_STATE_PRE					(1<<5)
#define CHARGE_STATE_FAST					(1<<4)
#define CHARGE_STATE_TRICK					(1<<3)
#define CHARGE_STATE_FULL					(1<<2)
#define BAT_DETECT_BIT						(1<<1)   //电池测试
#define BAT_CHECK_BIT						(1<<0)   //有无电池


#define LDO_LEVEL	4000

#if (LDO_LEVEL == 3300)
#warning "3.3v ldo"
#define BAT_MIN_VOLT_33_OPEN	472		//(0.3/2.6)*4096
#define BAT_MIN_VOLT_OPEN	372 			//(0.3/3.3)*4096
#define BAT_MAX_VOLT_OPEN	1923		//(1.55/3.3)*4096

#define BAT_MIN_VOLT_OPEN_SPE	372 			//(0.3/3.3)*4096

#define BAT_MAX_VOLT_CLOSE 2482		//(2/3.3*4096)        (1.8/3.3)*4096	
#define BAT_MAX_VOLT_CLOSE_CHANNEL_4		2234	//(1.8/3.3)*4096


#define BAT_LEVEL_LOW_TO_MIDD		1737			// 1.4     (1.4/3.3)*4096
#define BAT_LEVEL_MIDD_TO_HIGH		1750				//  1.41	(1.41/3.3)*4096

#define CHARGING_PRE_END_VOLT		1117			//(0.9/3.3)*4096
#define CHARGING_FAST_END_VOLT	1799			//(1.45/3.3)*4096
#define CHARGING_FAST_MAX_VOLT	1986			//(1.6/3.3)*4096

#define CHARGING_FAST_TEMP_END_VOLT	1799			// (1.45/3.3)*4096

#define DV_ENABLE_MIN_VOLT		1799						//  (1.45/3.3)*4096

#define BAT_LEVEL_12_CHARGING		1688		// 1.36   (1.36/3.3*4096)
#define BAT_LEVEL_23_CHARGING		1712		// 1.38    (1.38/3.3*4096)
#define BAT_LEVEL_34_CHARGING		1737		// 1.4   (1.4/3.3*4096)

#define BAT_INITIAL_VOLT_FULL		1824		// 1.47 (1.47/3.3*4096)

//output
#define MIN_VBAT_CHANNEL_1_IDLE	 2916  //(4.7/2=2.4V   2.4/3.3*4096)
#define MIN_VBAT_OUPUT				620	//(1/2=0.5	0.5/3.3*4096)
#define MIN_VBAT_OUTPUT_IDLE		620	// (1.3/2 = 0.65  0.65/3.3*4096)
#define	OUTPUT_SHOW_LEVEL_3		806	//(1.3/2  0.65/3.3*4096 )
#define	OUTPUT_SHOW_LEVEL_2		775	//(1.25/2 0.625/3.3*4096)	

#define	BAT_LEVEL_43_OUTPUT		1563      // 1.26 (1.26/3.3*4096)
#define 	BAT_LEVEL_32_OUTPUT		1526      //  1.23 (1.23/3.3*4096)
#define   BAT_LEVEL_21_OUTPUT		1501	//  1.21 (1.21/3.3*4096)

#define	BAT_LEVEL_43_IDLE		1613      // 1.3 (1.3/3.3*4096)
#define 	BAT_LEVEL_32_IDLE		1576      //  1.27 (1.27/3.3*4096)
#define   BAT_LEVEL_21_IDLE		1551	//  1.25 (1.25/3.3*4096)

//temperature adc
#define 	ADC_TEMP_MAX	1069    //55
#define	ADC_TEMP_MIN	3007	//0

#elif (LDO_LEVEL == 4000)
#warning "4v ldo"
#define BAT_MIN_VOLT_OPEN	307 			//(0.3/4)*4096
#define BAT_MAX_VOLT_OPEN	1587		//(1.55/4)*4096

#define BAT_MIN_VOLT_OPEN_SPE	307 			//(0.3/4)*4096

#define BAT_MAX_VOLT_CLOSE 2048		//(2/4*4096)
#define BAT_MAX_VOLT_CLOSE_CHANNEL_4		1843	//(1.8/4)*4096


#define BAT_LEVEL_LOW_TO_MIDD		1433			// 1.4     (1.4/4)*4096
#define BAT_LEVEL_MIDD_TO_HIGH		1443				//  1.41	(1.41/4)*4096

#define CHARGING_PRE_END_VOLT		921			//(0.9/4)*4096
#define CHARGING_FAST_END_VOLT	1484			//(1.45/4)*4096
#define CHARGING_FAST_MAX_VOLT	1638			//(1.6/4)*4096

#define CHARGING_FAST_TEMP_END_VOLT	1484			// (1.45/4)*4096

#define DV_ENABLE_MIN_VOLT		1484						//  (1.45/4)*4096

#define BAT_LEVEL_12_CHARGING		1392		// 1.36   (1.36/4*4096)
#define BAT_LEVEL_23_CHARGING		1413		// 1.38    (1.38/4*4096)
#define BAT_LEVEL_34_CHARGING		1433		// 1.4   (1.4/4*4096)

#define BAT_INITIAL_VOLT_FULL		1505		// 1.47 (1.47/4*4096)

//output
#define MIN_VBAT_CHANNEL_1_IDLE	 2457  //(4.7/2=2.4V   2.4/4*4096)
#define MIN_VBAT_OUPUT				512	//(1/2=0.5	0.5/4*4096)
#define MIN_VBAT_OUTPUT_IDLE		665	// (1.3/2 = 0.65  0.65/4*4096)
#define	OUTPUT_SHOW_LEVEL_3		665	//(1.3/2  0.65/4*4096 )
#define	OUTPUT_SHOW_LEVEL_2		640	//(1.25/2 0.625/4*4096)

#define	BAT_LEVEL_43_OUTPUT		1290      // 1.26 (1.26/4*4096)
#define 	BAT_LEVEL_32_OUTPUT		1259      //  1.23 (1.23/4*4096)
#define   BAT_LEVEL_21_OUTPUT		1239	//  1.21 (1.21/4*4096)

#define	BAT_LEVEL_43_IDLE		1331      // 1.3 (1.3/4*4096)
#define 	BAT_LEVEL_32_IDLE		1300      //  1.27 (1.27/4*4096)
#define   BAT_LEVEL_21_IDLE		1280	//  1.25 (1.25/4*4096)

//temperature adc
#define 	ADC_TEMP_MAX	1069    //55
#define	ADC_TEMP_MIN	3007	//0
#endif

#if 0
#define BAT_MIN_VOLT_33_OPEN	472		//(0.3/2.6)*4096
#define BAT_MIN_VOLT_OPEN	186 			//(0.3/3.3)*4096
#define BAT_MAX_VOLT_OPEN	1055		//(1.7/3.3)*4096


#define BAT_MAX_VOLT_CLOSE 1117		//(1.8/3.3)*4096	


#define BAT_LEVEL_LOW_TO_MIDD		868			// 1.4     (1.4/3.3)*4096
#define BAT_LEVEL_MIDD_TO_HIGH		875				//  1.41	(1.41/3.3)*4096

#define CHARGING_PRE_END_VOLT		558			//(0.9/3.3)*4096
#define CHARGING_FAST_END_VOLT	900			//(1.45/3.3)*4096
#define CHARGING_FAST_MAX_VOLT	993			//(1.6/3.3)*4096

#define CHARGING_FAST_TEMP_END_VOLT	900			// (1.45/3.3)*4096
#endif

#define BAT_LEVEL_LOW	1
#define BAT_LEVEL_MIDD	2
#define BAT_LEVEL_HIGH	3


#define MIN_BAT_LEVEL_UPDATE_INTERVAL	7324 //120000	2000/16.384
#define BAT_CHARGING_PULSE_TIME	30   //2000ms  2000/16.384
#define BAT_CHARGING_PULSE_DELAY_TIME	33//1100ms
#define BAT_CHARGING_DETECT_TIME	122
#define BAT_CHARGING_PWMOFF_DETECT_TIME	90		// 1.5S	

#define BAT_CHARGING_TEST_TIME	15	
#define BAT_CHARGING_TRICK_TIME	12

#define BAT_CHARGING_PRE_MAX_TIME	73242	//20min   (20*60*1000)/16.384
#define BAT_CHARGING_FAST_MAX_TIME	 1538085             //  7hour  (5*60*60*1000)/16.384
#define BAT_START_DV_CHECK			18310			   // 5min  (5*60*1000)/16.384

#define BAT_INITIAL_FULL_CHECK_TIME	10986			// 3min  (3*60*1000)/16.384
#define BAT_DV_CHECK_INTERVAL			305			//5s  5*1000/16.384
#define ADC_DV_VOLT						6			//5mv   0.005/3.3*4096


#define FAST_SKIP_COUNT	0
#define SUP_SKIP_COUNT		0
#define PRE_SKIP_COUNT		0
#define TRI_SKIP_COUNT		10
#define DUMMY_SKIP_COPUNT	0xFF

#define CHANNEL_VBAT_1	10
#define CHANNEL_VBAT_2 11
#define CHANNEL_VBAT_3	8
#define CHANNEL_VBAT_4	13


#define CHANNEL_TEMP_1		12
#define CHANNEL_TEMP_2		1

#define CHANNEL_20_RES	9


#define BAT_VALID_VALUE	787

#define SHOW_CHARGING_TICK	0x40

#define LED_DISPLAY_INTERVAL	122		// 2s 2*1000/16.384


//output
#define OUTPUT_STATUS_WAIT	0
#define OUTPUT_STATUS_NORMAL	1
#define OUTPUT_STATUS_STOP	2

#define OUTPUT_CHECK_INTERVAL		31	//(500/16.384	31)	


#define ENABLE_BOOST()	P25=1
#define DISABLE_BOOST()	P25=0

u32 getSysTick();
u32 getDiffTickFromNow(u32 ttick);
void ledHandler(void);
void delay_us(u16);
u32 getBatTick();
	
u16 getVbatAdc(u8 channel);
u16 getAverage(u8 channel);
u16 getBatTemp(u8 batNum);

void send(u16 sData);
void sendStr(char str[]);
void sendF(u32 sData);

void LED_OFF(u8 led);
void LED_ON(u8 led);

void delay_ms(u16 nus);


#endif
