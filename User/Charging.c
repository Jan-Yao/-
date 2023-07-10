#include "Charging.h"
#include "pin_mcu.h" 
#include "Flash.h" 
#include "ADC.h"
#include "I2C.h"

uint8_t  LedTimeOn = 0;
uint8_t  LedTimeOff = 0;
//uint8_t  FadapterYes = 0;
//uint8_t  FadapterNo = 0;
uint8_t  SystemState = 0;
uint8_t  BqSetDptr = 0;
uint8_t  FchargeOver = 0;
//uint8_t  FusbTestOver = 0;
uint8_t  OldChargeStep = 0;
uint8_t  ChargeStepBuf = 0;
uint8_t  BqTimeCnt = 0;
uint8_t  BatErrorCnt = 0;
uint8_t  FtempCoseOpen = 0;
//uint8_t  TempDetect = 0;
uint8_t  ChargeDisDptr = 0;
uint8_t  AdapterCnt = 0;
uint8_t  UsbVtestCnt = 0;
uint8_t  UsbVtestCnt1 = 0;
uint8_t  DisChargeDisDptr = 0;
uint8_t  UsbVCount = 0;
uint8_t  F_led = 0;
uint8_t  LedOntimeCnt = 0;
uint8_t  KeyBatVDataCount = 0;
uint8_t  OldDisChargeStep = 0;
uint8_t  DisChargeStepBuf = 0;
uint8_t  DisChargeStepCnt = 0;

uint16_t NTC_Count = 0;
uint16_t NTC_Sum = 0;
uint16_t NTC_Data = 0;
uint16_t TEMP_Sum = 0;
uint16_t TEMP_Data = 0;
uint16_t UsbVSum = 0;
uint16_t UsbVData = 0;
uint16_t KeyBatData = 0;
uint16_t BatErrorCnt1 = 0;
uint16_t ChargeStepCnt = 0;
uint16_t DisChargeDisTimeCnt_ON = 0;
uint16_t DisChargeDisTimeCnt_OFF = 0;
uint16_t DisChargeDisTimeCnt_LP_OFF = 0;

uint32_t ChargeTimeCnt = 0;
uint32_t ReChargeTimeErrCntBuf = 0;

/*********************************************************************
*功能：寄存器操作 充电使能
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void ChargeSwtich(uint8_t chargestate)
{
	if(chargestate==CHARGEENBLE)
	{
		BqData[1] &=0XCF;//11001111
		BqData[1] |=0X10;//00010000  （5-4Bit置位成01，使能充电）
		BqData[7] &=0XDF;//11011111   //bit5 置0 Vbat连接Vsys
	}
	else if(chargestate==CHARGEDISABLE)
	{
		BqData[1] &=0XCF;//11001111
		if(Key_P_ON == 1)  //开激光不断Vbat（防止适配器拔出）
		{
			BqData[7] &=0XDF;//11011111 
		}
		else
		{
			BqData[7] |=0X20;//00100000  //bit5 置1 Vbat连接断开
		}
	}	
	WriteReg(REG01,BqData[1]);
	WriteReg(REG07,BqData[7]);
}

/*********************************************************************
*功能：Stop充电IC 类似于重新赋值
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void StopBQ24195(void)
{
	BQCLRCE;
	WriteReg(REG00,0x37); //0011 0111  4.36V 3A
	WriteReg(REG01,0x00); //5-4Bit为00 Charge Disable
	WriteReg(REG02,0x00); 
	WriteReg(REG03,0x11); //0001 0001  预充电电流和截止电流都是256mA
	WriteReg(REG04,0xb2); //1011 0010  充电截止电压4.28V 预充电电压3V
	WriteReg(REG05,0x00); 
	WriteReg(REG06,0x03); //0000 0011  热调节120摄氏度
	WriteReg(REG07,0x00);	
}

/*********************************************************************
*功能：Sleep充电IC 将电池断开
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void EnterSleepProcess(void)
{
	BQCLRCE;	
	WriteReg(REG00,0x07); 
	WriteReg(REG01,0x00); 
	WriteReg(REG02,0x00); 
	WriteReg(REG03,0x11); 
	WriteReg(REG04,0xb3); 
	WriteReg(REG05,0x00); 
	WriteReg(REG06,0x03); 
	WriteReg(REG07,0x20);	//断开电池
}

/*********************************************************************
*功能：等待类电压检测
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void CycleTestBatV(void)  //while死等检测电压 为了得到当前电压值
{
	BVCON;		
	KeyBatVDataCount=0;   
	KeyBatData=0; 
	while(KeyBatVDataCount<16)
	{
		KeyBatVDataCount++;
		KeyBatData+=Get_Adc(ADC_Channel_2);	//电压采样	PA3 通道3		
	}
	KeyBatData>>=4;		
}

/*********************************************************************
*功能：充电电压检测分级
*参数：无
*返回值： 无
*备注：
*********************************************************************/
//uint8_t ChargeVlevelTest(uint16_t batv)
//{
//	uint8_t   batlevel;
//	
//	if(batv < (Bat0v50))
//	{
//		batlevel=R_G_FLASH;  
//	}
//	else if(batv < (Bat4v00))  //根据CTP更改 小于4V红灯亮常亮
//	{
//		batlevel=REDKEEPON; 
//	}
//	else
//	{
//		batlevel=GREENFLASHON;  
//	}	
//	
//	if(Key_P_ON == 1)
//	{
//		if(batlevel == R_G_FLASH)
//		{
//			LedTimeOff=LED4HZ;
//			LedTimeOn =LED4HZ;
//		}
//		else
//		{
//			LedTimeOff=LED2HZ;
//			LedTimeOn =LED2HZ;
//		}
//		if(ChargeDisDptr != YELLOWFLASHON)
//		{
//			ChargeDisDptr = YELLOWFLASHON;
//		}
//		batlevel=YELLOWFLASHON;
//	}
//	return 	batlevel;
//}

/*********************************************************************
*功能：放电电压检测分级
*参数：无
*返回值： 无
*备注：
*********************************************************************/
uint8_t DischargeVlevelTest(uint16_t batv)
{
//	uint8_t   batlevel;
//	if(batv>=Bat3v70)
//	{
//		batlevel=GREENKEEPON;
//	}
//	else if(batv>=Bat3v35)
//	{		
//		batlevel=YELLOWKEEPON;
//	}
//	else if(batv>=Bat3v13)
//	{
//		batlevel=REDKEEPON;
//	}
//	else if(batv>=Bat2v80)
//	{
//		batlevel=REDFLASHON; //其中2.8-3V阶段 不能让激光亮 只灯闪
//	}
//	else
//	{
//		batlevel=R_G_FLASH; //错误红绿灯交替
//	}
//	return 	batlevel;

	uint8_t   batlevel;
	if(batv>=Bat3v00)
	{
		batlevel=GREENKEEPON;
	}
	else if(batv>=Bat2v70)
	{
		batlevel=REDFLASHON;
	}
	else
	{
		batlevel=R_G_FLASH; //错误红绿灯交替
	}
	
	if(batlevel == R_G_FLASH)
	{
		LedTimeOff=LED4HZ;
		LedTimeOn =LED4HZ;
	}
	else
	{
		LedTimeOff=LED2HZ;
		LedTimeOn =LED2HZ;
	}
	return 	batlevel;
}

/*********************************************************************
*功能：充电显示判断
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void ChargeStepSure(void)
{
	uint16_t ChargeNum=0;
	
	if(ChargeDisDptr == GREENKEEPON)
	{
		ChargeNum = 300;
	}
	else
	{
		ChargeNum = 200;
	}
	
	if(OldChargeStep==ChargeStepBuf)
	{
		ChargeStepCnt++;
		if(ChargeStepCnt>ChargeNum)
		{
			ChargeStepCnt=0;
			ChargeDisDptr=OldChargeStep;
		}
	}
	else
	{
		ChargeStepCnt=0;
		OldChargeStep=ChargeStepBuf;
	}	
}

/*********************************************************************
*功能：放电显示判断
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void DisChargeStepSure(void)
{
	if(OldDisChargeStep==DisChargeStepBuf)
	{
		DisChargeStepCnt++;
		if(DisChargeStepCnt > 200)
		{
			DisChargeStepCnt=0;
			DisChargeDisDptr=OldDisChargeStep;
		}
	}
	else
	{
		DisChargeStepCnt=0;
		OldDisChargeStep=DisChargeStepBuf;
	}	
}

/*********************************************************************
*功能：充电过程判断
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void BqControlProcess(void) //充电步骤检测	
{
//	switch(BqSetDptr)  //充电步骤
//	{
//		case 0: 
//			StopBQ24195();		
//      ChargeSwtich(CHARGEDISABLE);		
//			ChargeTimeCnt=0;        //预充电时间	        
//			break;
//		case 1:
//			StopBQ24195();	    
//			ChargeSwtich(CHARGEDISABLE);      //关闭充电
//			BqSetDptr=2;  
//      BatErrorCnt=0;
//      BatErrorCnt1=0;
//      FchargeOver=0;			
//			break;
//		case 2:
//			if(FtempCoseOpen==1)        //温度超范围
//			{
//				BqSetDptr=4;             
//				break;
//			}
//			if((BatData<=Bat0v50)||(BatData>=Bat4v25))      //电池电压检测（电池大于4.25V 或 小于0.5V）
//			{
//				BatErrorCnt++;                  
//				if(BatErrorCnt>25)      //125ms
//				{
//					BatErrorCnt=0;
//				  BatErrorCnt1=0; 	
//					BqSetDptr=4;          
//					break;											
//        }								
//			}
//			else
//			{ 
//				if(BatData>=Bat4v05)     //电压检测(4.05<= <4.25V)
//				{
//					BatErrorCnt1++;  
//					if(BatErrorCnt1 > 600)   //等待3s 电池近似满电状态
//					{
//						BatErrorCnt=0;
//						BatErrorCnt1=0;						
//            BqSetDptr=4;
//            FchargeOver=1;						
//					}
//				}
//				else    //<4.05V
//				{
//					BatErrorCnt=0;
//					BatErrorCnt1=0;						
//					BqSetDptr=3;		
//					ChargeTimeCnt=0;
//				}										
//			}
//			break;
//		case 3:    //如果是4模式 正常充电的情况下拔出电池，必定要等30MIN？
//			BQCLRCE;					   //CE复位 充电使能 
//		
//      BqData[2] = ReadReg(REG02);	 //读取寄存器REG02	
//      BqData[8] = ReadReg(REG08);	 //读取寄存器REG08	  	

//			if(BatData<=Bat3v00) //电池电压小于3V 预充电模式 并累加时间
//			{
//				ChargeTimeCnt++;
//			}		
//			else
//			{
//				ChargeTimeCnt=0;  //预充电结束 清零
//			}	
//			
//			if(FtempCoseOpen==1)          //温度超范围
//			{
//				ChargeSwtich(CHARGEDISABLE);
//				BqSetDptr=4;  
//				break;
//			}								
//			
//			if((BqData[8]&0XC0)==USBHOST)   //& 1100 0000 USB调试模式 低电流
//			{
//				ReChargeTimeErrCntBuf=TIME150MIN;	//预充电时间延长至150MIN
//				
//				BqData[0]=0x39;               //0011 1001  4.44V  150mA
//        WriteReg(REG00,BqData[0]);				
//					
//				BqData[2]=CHARGE102MA;       //0000 0001 Bit0 置1 快充限制 512 * 20% = 102mA（修改了，直接赋值）
//				WriteReg(REG02,BqData[2]);

//				BqData[3]=0x00;               //预充电电流 128mA
//				WriteReg(REG03,BqData[3]);
//					
//				BqData[5]=0x00;               //充电截止不使能
//				WriteReg(REG05,BqData[5]);
//					
//				if(BatData>=Bat4v18)
//				{
//					ChargeSwtich(CHARGEDISABLE); 
//					BqSetDptr=5;  
//				}
//				else    //电压小于4.18V USBHOST模式下 充电 
//				{
//					ChargeSwtich(CHARGEENBLE);													
//				}									
//			}
//			else
//			{
//				ReChargeTimeErrCntBuf=TIME30MIN; //预充电等待时间
//				
//				BqData[0]=0x37;                  //0011 0111  4.36V 3A	
//        WriteReg(REG00,BqData[0]);	
//				
//				BqData[2]=CHARGE2176MA;          //0110 1000  快充电流限制2176mA（修改了，直接赋值）
//				WriteReg(REG02,BqData[2]);							
//					
//				BqData[3]=0x30;                  //0011 0000  预充电电流512mA 截止电流128mA
//				WriteReg(REG03,BqData[3]);
//				
//				BqData[5]=0x8a;                  //1000 1010  充电终止启用 充电安全计时器启用 快充定时器设置8小时
//				WriteReg(REG05,BqData[5]);	
//				
//				if((BqData[8]&0x30)==0x30)       //0011 0000  充电终止完成
//				{
//					ChargeSwtich(CHARGEDISABLE);
//					BqSetDptr=5;  	
//				}
//				else
//				{
//					ChargeSwtich(CHARGEENBLE);
//				}				
//			}

//			if(ChargeTimeCnt>ReChargeTimeErrCntBuf)  //预充电等待时间30MIN
//			{
//				ChargeSwtich(CHARGEDISABLE);			
//				return;					
//			}	
//			
//			break;
//		case 4:
//			//已经判断到异常 需要能够恢复
//      if((FtempCoseOpen==0)&&(BatData>=Bat0v50)&&(BatData<Bat4v05))  //BAT在范围内 可恢复
//			{
////				FchargeLedOff=0;
////				FchargeAlarm=0;
//				FchargeOver=0;
////				OldChargeStep=0;
////				ChargeStepBuf=0;
////				FsystemTestOK=0;
////				ChargeDisDptr=0;
////				DisChargeDisDptr=0;
//				FusbTestOver=0;
//				FadapterNo=0;
//				FadapterYes=0;
//				BqSetDptr=0;
//			}
//			else if(BatData>=Bat4v05)
//			{
//				FchargeOver=1;
//			}
//			break;
//		case 5:
//			BqTimeCnt++;
//      if(BqTimeCnt>200)  //实际测试 由于电感和电容 如果是电池BAT+断开 800ms左后B+电压才能稳定
//			{
//				BqTimeCnt=0;
//				BqSetDptr=1;		
//			}
//		  break;	
////		case 6:
////			EnterSleepProcess();
////		  break;	
//		default :
//			break;				
//		}
}

/*********************************************************************
*功能：NTC检测
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void CheckNTC(void)  
{
//	NTCON;
//	TEMPON;
//	NTC_Count++;
//	NTC_Sum += Get_Adc(ADC_Channel_2);
//	TEMP_Sum += Get_Adc(ADC_Channel_3);
//	if(NTC_Count>=16)
//	{
//		NTC_Count = 0;
//		NTC_Data = (NTC_Sum >>= 4);
//		TEMP_Data = (TEMP_Sum >>= 4);
//		NTC_Sum = 0;
//		TEMP_Sum = 0;
//		if((NTC_Data > NTC_0) || (NTC_Data < NTC_65))
//		{
//			FtempCoseOpen=1;
//		}
//		else
//		{
//			if(FtempCoseOpen == 1)
//			{
//				if((NTC_Data > NTC_60) && (NTC_Data < NTC_20))
//				{
//					FtempCoseOpen=0;
//				}
//			}
//			else
//			{
//				FtempCoseOpen=0;
//			}
//		}
////热敏电阻		
//		if(TEMP_Data < TEMP_75)  //温度大于75摄氏度   <170
//		{
//			TempDetect = 2;
//		}
//		else if(TEMP_Data < TEMP_50)  //温度大于50摄氏度  <300
//		{
//			if(TempDetect != 2)
//			{
//				TempDetect = 1;
//			}
//		}
//		else
//		{
//			if((TEMP_Data > TEMP_40) && (TEMP_Data < TEMP_0))  //370< <700
//			{
//				if((Switch_ON == 0) && (TempDetect == 2))
//				{
//					TempDetect = 0;
//				}
//				else if(TempDetect == 1)
//				{
//					TempDetect = 0;
//				}
//			}
//		}
//	}
}

/*********************************************************************
*功能：适配器检测
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void CheckAdaptIn(void)  //适配器检测
{
//	if(ADAPTIO==0)  //适配器IO口 检测==0 
//	{
//		if(FadapterYes==0)
//		{
//			AdapterCnt++;        
//			if(AdapterCnt>=5)   //25ms 检测充电器是否完全插入
//			{
//				AdapterCnt=0;     //清空计数
//				FadapterYes=1;    //适配器确认插入标志位
//				FadapterNo=0;		
//				FusbTestOver=0;	
//				SystemState=CHARGER;	
//			}
//		}
//		else 
//		{
//			AdapterCnt=0;
//		}					
//	}
//	else
//	{
//		if(FadapterNo==0)         //拔出   除非适配器变更 否则只进入一次
//		{
//			AdapterCnt++;           
//			if(AdapterCnt>=20)      //1s 检测充电器是否完全拔出
//			{
//				AdapterCnt=0;
//				FadapterYes=0;
//				FadapterNo=1;         //适配器确认拔出标志位
//				BqSetDptr=0;	        //关闭充电													
//				FusbTestOver=0;
//				SystemState=DISCHARGER;
//				DisChargeDisTimeCnt_ON=0; //3s显示 在此清零
//				DisChargeDisTimeCnt_OFF=0;
//			}
//		}
//		else
//		{
//			AdapterCnt=0;
//		}
//	}

//	if(FusbTestOver==1)   //测试结束 跳出 避免下面的程序重复执行 ADC重复采样浪费时间
//	{
//		if(SystemState == CHARGER)
//		{
//			if(Key_P_ON != 1)
//			{
//				LedTimeOff=LED2HZ;
//			  LedTimeOn =LED2HZ;
//			}
//			BVCON;
//			BatVfilter();	
//		}
//		else
//		{
//			LedTimeOff=LED4HZ;
//			LedTimeOn =LED4HZ;
//			BVCON;
//			if(ADC_OFF == 0)
//			{
//				BatVfilter();	
//			}
//		}
//	}
//  else
//	{
//		if(FadapterYes==1)  //充电步骤7 会将FsystemTestOK和FadapterYes回零（FsystemTestOK应该可以省略）
//		{
//			//适配器确定已经接入
//			UsbVSum+=Get_Adc(ADC_Channel_0);	      //usb 电压检测
//			UsbVCount++;                            //连续采样
//			if(UsbVCount>=8)   // 8*5 = 40ms
//			{
//				UsbVCount=0;
//				UsbVData=(UsbVSum>>=3);             //连续8次 取平均数
//				UsbVSum=0;
//				if((UsbVData>=USB4V50)&&(UsbVData<USB5V50))  //适配器电压范围
//				{
//					if(++UsbVtestCnt>10)   //40 * 10 = 400ms
//					{
//						UsbVtestCnt=0;
//						UsbVtestCnt1=0;
//						FusbTestOver=1;
//						BqSetDptr=1;     //开始充电检测 能正常充电的流程: 1 -> 2 -> 3									
//						//在此判断 上电电压是否大于4.05V  如果大于4.05V 则需要常亮LED 不充电
//            DisChargeDisDptr=0;
//						CycleTestBatV();                   //死等电压检测  应该还是被拉低的电量 不确定断开BAT
//						BatData = KeyBatData;
//						if(KeyBatData>=Bat4v05)            //电池电压 >= 4.05V
//						{
////  						StopBQ24195();	                 
////		  				ChargeSwtich(CHARGEDISABLE);   //不确定是否要加
//							FchargeOver=1;  
//							BqSetDptr=4;
//						}
//						else
//						{
//              if(ChargeDisDptr == 0)
//							{
//								ChargeDisDptr=ChargeVlevelTest(KeyBatData);  //充电指示灯检测
//							}
//						}
//					}			
//				}
//				else   //适配器电压过低
//				{
//					if(++UsbVtestCnt1>10)
//					{
//						UsbVtestCnt=0;
//						UsbVtestCnt1=0;
//						FusbTestOver=1;
//						BqSetDptr=0;             //关闭充电														
//					}															
//				}
//			}					
//		}
//		else if(FadapterNo==1) 
//		{
//			ChargeDisDptr=0;   //清除充电灯显示的标志位
//			CycleTestBatV();   //while版 电量检测 死等电压检测
//			if(BatData == 0)
//			{
//				BatData = KeyBatData;
//			}
//			DisChargeDisDptr=DischargeVlevelTest(KeyBatData);  //放电电量检测						
//			FusbTestOver = 1;
//		}
//	}	
}

/*********************************************************************
*功能：显示过程
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void DisplayProcess(void)
{
	uint8_t  FledFlashStart=0;
	
	if(SystemState==DISCHARGER)  //判断系统是否是在放电
	{
		if(((DisChargeDisTimeCnt_ON > 20) && (DisChargeDisTimeCnt_ON < 600)) 
		|| ((DisChargeDisTimeCnt_OFF > 20) && (DisChargeDisTimeCnt_OFF < 600)) 
		|| ((DisChargeDisTimeCnt_LP_OFF > 20) && (DisChargeDisTimeCnt_LP_OFF < 600)))  //采样延迟
		{
			if(DisChargeDisDptr == LEDOFF)
			{
				DisChargeDisDptr=DischargeVlevelTest(BatData);
			}
			else
			{
				DisChargeStepBuf=DischargeVlevelTest(BatData);
		    DisChargeStepSure();
			}
		}
		if(Switch_ON == 1)
		{
			DisChargeDisTimeCnt_OFF=0;
			if(Vol_Delay_ON == 1)
			{
				DisChargeDisTimeCnt_ON = 0;
				DisChargeDisTimeCnt_LP_OFF++;
				if(DisChargeDisTimeCnt_LP_OFF > 600)
				{
					DisChargeDisTimeCnt_LP_OFF=600;
				  DisChargeDisDptr=LEDOFF; 
				}
			}
			else
			{
				DisChargeDisTimeCnt_LP_OFF = 0;
				DisChargeDisTimeCnt_ON++;
				if(DisChargeDisTimeCnt_ON>600) //适配器拔出时，3s显示电量
				{
					if((DischargeVlevelTest(BatData) == REDFLASHON) || (DisChargeDisDptr == REDFLASHON))
					{
						DisChargeDisTimeCnt_ON = 0;  //3.13V报警常亮，但是恢复没法做到显示3s后关
					}
					else
					{
						DisChargeDisTimeCnt_ON=600;
						DisChargeDisDptr=LEDOFF;       //放电显示关 3s关
					}
				}
			}
		}
		else
		{
		  DisChargeDisTimeCnt_ON = 0;
			DisChargeDisTimeCnt_LP_OFF = 0;
			DisChargeDisTimeCnt_OFF++;
			if(DisChargeDisTimeCnt_OFF>600) //适配器拔出时，3s显示电量
			{
				DisChargeDisTimeCnt_OFF=600;
				DisChargeDisDptr=LEDOFF;       //放电显示关 3s关
			}
		}
		
		switch(DisChargeDisDptr)
		{
			case LEDOFF:
				LEDROFF;   //关红
			  LEDGOFF;   //关绿
			  break;
			case REDFLASHON:  //红灯闪烁
				LEDGOFF;
				FledFlashStart=1;
				if(F_led==1)
				{
					LEDRON;
				}
				else
				{
					LEDROFF;
				}
				break;
			case REDKEEPON:    //红灯常亮
				LEDRON;
				LEDGOFF;
				break;
			case YELLOWKEEPON:  //黄灯常亮
				LEDRON;
				LEDGON;
				break;
			case GREENKEEPON:   //绿灯常亮
				LEDROFF;
				LEDGON;
				break;
			case R_G_FLASH:     //红绿灯交替
				FledFlashStart=1;
				if(F_led==1)
				{
					LEDGON;
					LEDROFF;									 
				}
				else
				{
					LEDGOFF;
					LEDRON;									 
				}
				break;
			default :
				break;								 
			}
  }
//	else    //充电过程
//	{
//		//55%---3.73V  32.5%--3.55v   1%--3.3v
//		if((UsbVData>=USB4V50)&&(UsbVData<USB5V50))  //USB电压异常
//		{
//			if((FchargeOver==1)&&(Key_P_ON!=1))
//			{
//				if(ChargeDisDptr == 0)
//				{
//					ChargeDisDptr=GREENKEEPON;   //green keep on  绿灯常亮	
//				}
//				else
//				{
//					ChargeStepBuf=GREENKEEPON;   //green keep on  绿灯常亮	
//				}
//			}
//			else                //充电过程电量显示  和 适配器那的电量检测有一些冲突（看实验是否可以去除） 但这个检测是充电实时检测
//			{
//				ChargeStepBuf=ChargeVlevelTest(BatData);					
//			}	
//		}
//		else           
//		{
//			ChargeStepBuf=LEDOFF; 										
//		}			      
//		ChargeStepSure();         //第一次存在OldChargeStep = 0 将ChargeStepBuf赋值给Old 但是实际显示是使用的CheckAdaptIn()检测的值ChargeStep
//                              //如果1s稳定 ChargeStepBuf不突变 ChargeDisDptr为Old值（即ChargeStepBuf的值）一直在突变的话 延迟显示 直至稳定
//		switch(ChargeDisDptr)     //ChargeStepBuf突变 不更新ChargeDisDptr 除了ChargeDisable操作
//		{
//			case LEDOFF:
//				LEDROFF;
//				LEDGOFF;
//				break;
//			case REDKEEPON:  //红灯常亮
//					LEDRON;
//					LEDGOFF;								 
//				break;
//			case GREENFLASHON: //绿灯闪烁
//				LEDROFF;
//				FledFlashStart=1;
//				if(F_led==1)
//				{
//					LEDGON;
//				}
//				else
//				{
//					LEDGOFF;
//				}									 
//				break;
//			case GREENKEEPON:   //绿灯常亮
//				LEDROFF;
//				LEDGON;
//				break;
//			case R_G_FLASH:     //红绿交替
//				FledFlashStart=1;
//				if(F_led==1)
//				{
//					LEDGON;
//					LEDROFF;
//				}
//				else
//				{
//					LEDGOFF;
//					LEDRON;									 
//				}								   
//				break;
//		  case YELLOWFLASHON:
//				FledFlashStart=1;
//				if(F_led==1)
//				{
//					LEDRON;
//					LEDGON;
//				}
//				else
//				{
//					LEDROFF;
//					LEDGOFF;
//				}	
//				break;
//			default :
//				break;										
//		}
//  }
	
  if(FledFlashStart==1)
	{
		if(F_led==1)
	  {
			LedOntimeCnt++;
	    if(LedOntimeCnt>LedTimeOn)
	    {
				LedOntimeCnt=0;
	      F_led=0;
	    }
	  }
		else
	  {
			LedOntimeCnt++;
	    if(LedOntimeCnt>LedTimeOff)
	    {
				LedOntimeCnt=0;
	      F_led=1;
			}
		}
	}
	else
	{
		F_led=0;
		LedOntimeCnt=0;
	}	
}
