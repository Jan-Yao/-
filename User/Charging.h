#ifndef __Charging_H_
#define __Charging_H_   
	
#include "stm32f0xx.h"

//#define  Bat0v50		   80  + SysPar.SysStruct.Bat_V_Error
#define  Bat2v70		   270 + SysPar.SysStruct.Bat_V_Error
//#define  Bat2v80		   280 + SysPar.SysStruct.Bat_V_Error
#define  Bat3v00		   300 + SysPar.SysStruct.Bat_V_Error
//#define  Bat3v13		   313 + SysPar.SysStruct.Bat_V_Error
#define  Bat3v20		   320 + SysPar.SysStruct.Bat_V_Error
//#define  Bat3v35		   335 + SysPar.SysStruct.Bat_V_Error
//#define  Bat3v45		   345 + SysPar.SysStruct.Bat_V_Error
//#define  Bat3v60		   360 + SysPar.SysStruct.Bat_V_Error
//#define  Bat3v70		   370 + SysPar.SysStruct.Bat_V_Error
//#define  Bat3v78		   378 + SysPar.SysStruct.Bat_V_Error
//#define  Bat4v00		   400 + SysPar.SysStruct.Bat_V_Error
//#define  Bat4v05		   405 + SysPar.SysStruct.Bat_V_Error
//#define  Bat4v10		   410 + SysPar.SysStruct.Bat_V_Error
//#define  Bat4v18		   418 + SysPar.SysStruct.Bat_V_Error
//#define  Bat4v25		   425 + SysPar.SysStruct.Bat_V_Error
#define  Bat4v50		   450 + SysPar.SysStruct.Bat_V_Error

#define  USB4V50		   220//450
#define  USB5V50		   370//550

#define  NTC_0		     700
#define  NTC_20		     500
#define  NTC_60	       200
#define  NTC_65	       150

#define  TEMP_0		     700
#define  TEMP_40		   370  
#define  TEMP_50		   300  
#define  TEMP_65	     210  
#define  TEMP_75	     170  

#define  USBHOST		   0X40
//快充电流
#define  CHARGE512MA   0X00 
#define  CHARGE102MA   0X01 //512mA*20% =102MA
#define  CHARGE1024MA  0X20
#define  CHARGE2112MA  0x64
#define  CHARGE2304MA  0x70
#define  CHARGE2176MA  0x68
#define  CHARGE1500MA  0x40
#define  CHARGE1800MA  0x50
#define  CHARGE1200MA  0x30

#define  DISCHARGER		 0
#define  CHARGER		   1

#define  CHARGEENBLE   0
#define  CHARGEDISABLE 1

#define  LEDOFF        0
#define  REDFLASHON    1
#define  REDKEEPON     2
#define  YELLOWKEEPON  3
#define  GREENKEEPON   4
#define  R_G_FLASH     5
//充电时特有LED显示
#define  YELLOWFLASHON 6   
#define  GREENFLASHON  3

#define  TIME30MIN     360000     //预充电等待时间   
#define  TIME150MIN    1800000

#define  LED2HZ        50                
#define  LED4HZ			   25

#define  LEDRON	    GPIO_ResetBits(LED_PWR_R_GBIO,LED_PWR_R_PIN)
#define  LEDROFF	  GPIO_SetBits(LED_PWR_R_GBIO,LED_PWR_R_PIN)

#define  LEDGON	    GPIO_ResetBits(LED_PWR_G_GBIO,LED_PWR_G_PIN)
#define  LEDGOFF	  GPIO_SetBits(LED_PWR_G_GBIO,LED_PWR_G_PIN)

#define  BVCON	    GPIO_ResetBits(NTC_EN_GBIO,NTC_EN_PIN)
#define  BVCOFF	    GPIO_SetBits(NTC_EN_GBIO,NTC_EN_PIN)

#define  TEMPON	    GPIO_ResetBits(TEMP_EN_GBIO,TEMP_EN_PIN)
#define  TEMPOFF	  GPIO_SetBits(TEMP_EN_GBIO,TEMP_EN_PIN)

//#define  NTCON	    GPIO_SetBits(NTC_EN_GBIO,NTC_EN_PIN)
//#define  NTCOFF	    GPIO_ResetBits(NTC_EN_GBIO,NTC_EN_PIN)

#define  BQSETCE	  GPIO_SetBits(CH_EN_GBIO,CH_EN_PIN)
#define  BQCLRCE	  GPIO_ResetBits(CH_EN_GBIO,CH_EN_PIN)

#define  ADAPTIO	  GPIO_ReadInputDataBit(VB_WAK_GBIO,VB_WAK_PIN)

//extern uint8_t  FadapterYes;
//extern uint8_t  FadapterNo;
//extern uint8_t  FusbTestOver;
extern uint8_t  Key_P_ON;
extern uint8_t  Switch_ON;
extern uint8_t  ADC_OFF;
extern uint8_t  Vol_Delay_ON;
extern uint16_t DisChargeDisTimeCnt_OFF;

void BqControlProcess(void);
void CheckAdaptIn(void); 
void CheckNTC(void);
void DisplayProcess(void);
void EnterSleepProcess(void);

void CycleTestBatV(void);

#endif
