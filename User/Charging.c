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
*���ܣ��Ĵ������� ���ʹ��
*��������
*����ֵ�� ��
*��ע��
*********************************************************************/
void ChargeSwtich(uint8_t chargestate)
{
	if(chargestate==CHARGEENBLE)
	{
		BqData[1] &=0XCF;//11001111
		BqData[1] |=0X10;//00010000  ��5-4Bit��λ��01��ʹ�ܳ�磩
		BqData[7] &=0XDF;//11011111   //bit5 ��0 Vbat����Vsys
	}
	else if(chargestate==CHARGEDISABLE)
	{
		BqData[1] &=0XCF;//11001111
		if(Key_P_ON == 1)  //�����ⲻ��Vbat����ֹ�������γ���
		{
			BqData[7] &=0XDF;//11011111 
		}
		else
		{
			BqData[7] |=0X20;//00100000  //bit5 ��1 Vbat���ӶϿ�
		}
	}	
	WriteReg(REG01,BqData[1]);
	WriteReg(REG07,BqData[7]);
}

/*********************************************************************
*���ܣ�Stop���IC ���������¸�ֵ
*��������
*����ֵ�� ��
*��ע��
*********************************************************************/
void StopBQ24195(void)
{
	BQCLRCE;
	WriteReg(REG00,0x37); //0011 0111  4.36V 3A
	WriteReg(REG01,0x00); //5-4BitΪ00 Charge Disable
	WriteReg(REG02,0x00); 
	WriteReg(REG03,0x11); //0001 0001  Ԥ�������ͽ�ֹ��������256mA
	WriteReg(REG04,0xb2); //1011 0010  ����ֹ��ѹ4.28V Ԥ����ѹ3V
	WriteReg(REG05,0x00); 
	WriteReg(REG06,0x03); //0000 0011  �ȵ���120���϶�
	WriteReg(REG07,0x00);	
}

/*********************************************************************
*���ܣ�Sleep���IC ����ضϿ�
*��������
*����ֵ�� ��
*��ע��
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
	WriteReg(REG07,0x20);	//�Ͽ����
}

/*********************************************************************
*���ܣ��ȴ����ѹ���
*��������
*����ֵ�� ��
*��ע��
*********************************************************************/
void CycleTestBatV(void)  //while���ȼ���ѹ Ϊ�˵õ���ǰ��ѹֵ
{
	BVCON;		
	KeyBatVDataCount=0;   
	KeyBatData=0; 
	while(KeyBatVDataCount<16)
	{
		KeyBatVDataCount++;
		KeyBatData+=Get_Adc(ADC_Channel_2);	//��ѹ����	PA3 ͨ��3		
	}
	KeyBatData>>=4;		
}

/*********************************************************************
*���ܣ�����ѹ���ּ�
*��������
*����ֵ�� ��
*��ע��
*********************************************************************/
//uint8_t ChargeVlevelTest(uint16_t batv)
//{
//	uint8_t   batlevel;
//	
//	if(batv < (Bat0v50))
//	{
//		batlevel=R_G_FLASH;  
//	}
//	else if(batv < (Bat4v00))  //����CTP���� С��4V���������
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
*���ܣ��ŵ��ѹ���ּ�
*��������
*����ֵ�� ��
*��ע��
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
//		batlevel=REDFLASHON; //����2.8-3V�׶� �����ü����� ֻ����
//	}
//	else
//	{
//		batlevel=R_G_FLASH; //������̵ƽ���
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
		batlevel=R_G_FLASH; //������̵ƽ���
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
*���ܣ������ʾ�ж�
*��������
*����ֵ�� ��
*��ע��
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
*���ܣ��ŵ���ʾ�ж�
*��������
*����ֵ�� ��
*��ע��
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
*���ܣ��������ж�
*��������
*����ֵ�� ��
*��ע��
*********************************************************************/
void BqControlProcess(void) //��粽����	
{
//	switch(BqSetDptr)  //��粽��
//	{
//		case 0: 
//			StopBQ24195();		
//      ChargeSwtich(CHARGEDISABLE);		
//			ChargeTimeCnt=0;        //Ԥ���ʱ��	        
//			break;
//		case 1:
//			StopBQ24195();	    
//			ChargeSwtich(CHARGEDISABLE);      //�رճ��
//			BqSetDptr=2;  
//      BatErrorCnt=0;
//      BatErrorCnt1=0;
//      FchargeOver=0;			
//			break;
//		case 2:
//			if(FtempCoseOpen==1)        //�¶ȳ���Χ
//			{
//				BqSetDptr=4;             
//				break;
//			}
//			if((BatData<=Bat0v50)||(BatData>=Bat4v25))      //��ص�ѹ��⣨��ش���4.25V �� С��0.5V��
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
//				if(BatData>=Bat4v05)     //��ѹ���(4.05<= <4.25V)
//				{
//					BatErrorCnt1++;  
//					if(BatErrorCnt1 > 600)   //�ȴ�3s ��ؽ�������״̬
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
//		case 3:    //�����4ģʽ ������������°γ���أ��ض�Ҫ��30MIN��
//			BQCLRCE;					   //CE��λ ���ʹ�� 
//		
//      BqData[2] = ReadReg(REG02);	 //��ȡ�Ĵ���REG02	
//      BqData[8] = ReadReg(REG08);	 //��ȡ�Ĵ���REG08	  	

//			if(BatData<=Bat3v00) //��ص�ѹС��3V Ԥ���ģʽ ���ۼ�ʱ��
//			{
//				ChargeTimeCnt++;
//			}		
//			else
//			{
//				ChargeTimeCnt=0;  //Ԥ������ ����
//			}	
//			
//			if(FtempCoseOpen==1)          //�¶ȳ���Χ
//			{
//				ChargeSwtich(CHARGEDISABLE);
//				BqSetDptr=4;  
//				break;
//			}								
//			
//			if((BqData[8]&0XC0)==USBHOST)   //& 1100 0000 USB����ģʽ �͵���
//			{
//				ReChargeTimeErrCntBuf=TIME150MIN;	//Ԥ���ʱ���ӳ���150MIN
//				
//				BqData[0]=0x39;               //0011 1001  4.44V  150mA
//        WriteReg(REG00,BqData[0]);				
//					
//				BqData[2]=CHARGE102MA;       //0000 0001 Bit0 ��1 ������� 512 * 20% = 102mA���޸��ˣ�ֱ�Ӹ�ֵ��
//				WriteReg(REG02,BqData[2]);

//				BqData[3]=0x00;               //Ԥ������ 128mA
//				WriteReg(REG03,BqData[3]);
//					
//				BqData[5]=0x00;               //����ֹ��ʹ��
//				WriteReg(REG05,BqData[5]);
//					
//				if(BatData>=Bat4v18)
//				{
//					ChargeSwtich(CHARGEDISABLE); 
//					BqSetDptr=5;  
//				}
//				else    //��ѹС��4.18V USBHOSTģʽ�� ��� 
//				{
//					ChargeSwtich(CHARGEENBLE);													
//				}									
//			}
//			else
//			{
//				ReChargeTimeErrCntBuf=TIME30MIN; //Ԥ���ȴ�ʱ��
//				
//				BqData[0]=0x37;                  //0011 0111  4.36V 3A	
//        WriteReg(REG00,BqData[0]);	
//				
//				BqData[2]=CHARGE2176MA;          //0110 1000  ����������2176mA���޸��ˣ�ֱ�Ӹ�ֵ��
//				WriteReg(REG02,BqData[2]);							
//					
//				BqData[3]=0x30;                  //0011 0000  Ԥ������512mA ��ֹ����128mA
//				WriteReg(REG03,BqData[3]);
//				
//				BqData[5]=0x8a;                  //1000 1010  �����ֹ���� ��簲ȫ��ʱ������ ��䶨ʱ������8Сʱ
//				WriteReg(REG05,BqData[5]);	
//				
//				if((BqData[8]&0x30)==0x30)       //0011 0000  �����ֹ���
//				{
//					ChargeSwtich(CHARGEDISABLE);
//					BqSetDptr=5;  	
//				}
//				else
//				{
//					ChargeSwtich(CHARGEENBLE);
//				}				
//			}

//			if(ChargeTimeCnt>ReChargeTimeErrCntBuf)  //Ԥ���ȴ�ʱ��30MIN
//			{
//				ChargeSwtich(CHARGEDISABLE);			
//				return;					
//			}	
//			
//			break;
//		case 4:
//			//�Ѿ��жϵ��쳣 ��Ҫ�ܹ��ָ�
//      if((FtempCoseOpen==0)&&(BatData>=Bat0v50)&&(BatData<Bat4v05))  //BAT�ڷ�Χ�� �ɻָ�
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
//      if(BqTimeCnt>200)  //ʵ�ʲ��� ���ڵ�к͵��� ����ǵ��BAT+�Ͽ� 800ms���B+��ѹ�����ȶ�
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
*���ܣ�NTC���
*��������
*����ֵ�� ��
*��ע��
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
////��������		
//		if(TEMP_Data < TEMP_75)  //�¶ȴ���75���϶�   <170
//		{
//			TempDetect = 2;
//		}
//		else if(TEMP_Data < TEMP_50)  //�¶ȴ���50���϶�  <300
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
*���ܣ����������
*��������
*����ֵ�� ��
*��ע��
*********************************************************************/
void CheckAdaptIn(void)  //���������
{
//	if(ADAPTIO==0)  //������IO�� ���==0 
//	{
//		if(FadapterYes==0)
//		{
//			AdapterCnt++;        
//			if(AdapterCnt>=5)   //25ms ��������Ƿ���ȫ����
//			{
//				AdapterCnt=0;     //��ռ���
//				FadapterYes=1;    //������ȷ�ϲ����־λ
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
//		if(FadapterNo==0)         //�γ�   ������������� ����ֻ����һ��
//		{
//			AdapterCnt++;           
//			if(AdapterCnt>=20)      //1s ��������Ƿ���ȫ�γ�
//			{
//				AdapterCnt=0;
//				FadapterYes=0;
//				FadapterNo=1;         //������ȷ�ϰγ���־λ
//				BqSetDptr=0;	        //�رճ��													
//				FusbTestOver=0;
//				SystemState=DISCHARGER;
//				DisChargeDisTimeCnt_ON=0; //3s��ʾ �ڴ�����
//				DisChargeDisTimeCnt_OFF=0;
//			}
//		}
//		else
//		{
//			AdapterCnt=0;
//		}
//	}

//	if(FusbTestOver==1)   //���Խ��� ���� ��������ĳ����ظ�ִ�� ADC�ظ������˷�ʱ��
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
//		if(FadapterYes==1)  //��粽��7 �ὫFsystemTestOK��FadapterYes���㣨FsystemTestOKӦ�ÿ���ʡ�ԣ�
//		{
//			//������ȷ���Ѿ�����
//			UsbVSum+=Get_Adc(ADC_Channel_0);	      //usb ��ѹ���
//			UsbVCount++;                            //��������
//			if(UsbVCount>=8)   // 8*5 = 40ms
//			{
//				UsbVCount=0;
//				UsbVData=(UsbVSum>>=3);             //����8�� ȡƽ����
//				UsbVSum=0;
//				if((UsbVData>=USB4V50)&&(UsbVData<USB5V50))  //��������ѹ��Χ
//				{
//					if(++UsbVtestCnt>10)   //40 * 10 = 400ms
//					{
//						UsbVtestCnt=0;
//						UsbVtestCnt1=0;
//						FusbTestOver=1;
//						BqSetDptr=1;     //��ʼ����� ��������������: 1 -> 2 -> 3									
//						//�ڴ��ж� �ϵ��ѹ�Ƿ����4.05V  �������4.05V ����Ҫ����LED �����
//            DisChargeDisDptr=0;
//						CycleTestBatV();                   //���ȵ�ѹ���  Ӧ�û��Ǳ����͵ĵ��� ��ȷ���Ͽ�BAT
//						BatData = KeyBatData;
//						if(KeyBatData>=Bat4v05)            //��ص�ѹ >= 4.05V
//						{
////  						StopBQ24195();	                 
////		  				ChargeSwtich(CHARGEDISABLE);   //��ȷ���Ƿ�Ҫ��
//							FchargeOver=1;  
//							BqSetDptr=4;
//						}
//						else
//						{
//              if(ChargeDisDptr == 0)
//							{
//								ChargeDisDptr=ChargeVlevelTest(KeyBatData);  //���ָʾ�Ƽ��
//							}
//						}
//					}			
//				}
//				else   //��������ѹ����
//				{
//					if(++UsbVtestCnt1>10)
//					{
//						UsbVtestCnt=0;
//						UsbVtestCnt1=0;
//						FusbTestOver=1;
//						BqSetDptr=0;             //�رճ��														
//					}															
//				}
//			}					
//		}
//		else if(FadapterNo==1) 
//		{
//			ChargeDisDptr=0;   //���������ʾ�ı�־λ
//			CycleTestBatV();   //while�� ������� ���ȵ�ѹ���
//			if(BatData == 0)
//			{
//				BatData = KeyBatData;
//			}
//			DisChargeDisDptr=DischargeVlevelTest(KeyBatData);  //�ŵ�������						
//			FusbTestOver = 1;
//		}
//	}	
}

/*********************************************************************
*���ܣ���ʾ����
*��������
*����ֵ�� ��
*��ע��
*********************************************************************/
void DisplayProcess(void)
{
	uint8_t  FledFlashStart=0;
	
	if(SystemState==DISCHARGER)  //�ж�ϵͳ�Ƿ����ڷŵ�
	{
		if(((DisChargeDisTimeCnt_ON > 20) && (DisChargeDisTimeCnt_ON < 600)) 
		|| ((DisChargeDisTimeCnt_OFF > 20) && (DisChargeDisTimeCnt_OFF < 600)) 
		|| ((DisChargeDisTimeCnt_LP_OFF > 20) && (DisChargeDisTimeCnt_LP_OFF < 600)))  //�����ӳ�
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
				if(DisChargeDisTimeCnt_ON>600) //�������γ�ʱ��3s��ʾ����
				{
					if((DischargeVlevelTest(BatData) == REDFLASHON) || (DisChargeDisDptr == REDFLASHON))
					{
						DisChargeDisTimeCnt_ON = 0;  //3.13V�������������ǻָ�û��������ʾ3s���
					}
					else
					{
						DisChargeDisTimeCnt_ON=600;
						DisChargeDisDptr=LEDOFF;       //�ŵ���ʾ�� 3s��
					}
				}
			}
		}
		else
		{
		  DisChargeDisTimeCnt_ON = 0;
			DisChargeDisTimeCnt_LP_OFF = 0;
			DisChargeDisTimeCnt_OFF++;
			if(DisChargeDisTimeCnt_OFF>600) //�������γ�ʱ��3s��ʾ����
			{
				DisChargeDisTimeCnt_OFF=600;
				DisChargeDisDptr=LEDOFF;       //�ŵ���ʾ�� 3s��
			}
		}
		
		switch(DisChargeDisDptr)
		{
			case LEDOFF:
				LEDROFF;   //�غ�
			  LEDGOFF;   //����
			  break;
			case REDFLASHON:  //�����˸
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
			case REDKEEPON:    //��Ƴ���
				LEDRON;
				LEDGOFF;
				break;
			case YELLOWKEEPON:  //�ƵƳ���
				LEDRON;
				LEDGON;
				break;
			case GREENKEEPON:   //�̵Ƴ���
				LEDROFF;
				LEDGON;
				break;
			case R_G_FLASH:     //���̵ƽ���
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
//	else    //������
//	{
//		//55%---3.73V  32.5%--3.55v   1%--3.3v
//		if((UsbVData>=USB4V50)&&(UsbVData<USB5V50))  //USB��ѹ�쳣
//		{
//			if((FchargeOver==1)&&(Key_P_ON!=1))
//			{
//				if(ChargeDisDptr == 0)
//				{
//					ChargeDisDptr=GREENKEEPON;   //green keep on  �̵Ƴ���	
//				}
//				else
//				{
//					ChargeStepBuf=GREENKEEPON;   //green keep on  �̵Ƴ���	
//				}
//			}
//			else                //�����̵�����ʾ  �� �������ǵĵ��������һЩ��ͻ����ʵ���Ƿ����ȥ���� ���������ǳ��ʵʱ���
//			{
//				ChargeStepBuf=ChargeVlevelTest(BatData);					
//			}	
//		}
//		else           
//		{
//			ChargeStepBuf=LEDOFF; 										
//		}			      
//		ChargeStepSure();         //��һ�δ���OldChargeStep = 0 ��ChargeStepBuf��ֵ��Old ����ʵ����ʾ��ʹ�õ�CheckAdaptIn()����ֵChargeStep
//                              //���1s�ȶ� ChargeStepBuf��ͻ�� ChargeDisDptrΪOldֵ����ChargeStepBuf��ֵ��һֱ��ͻ��Ļ� �ӳ���ʾ ֱ���ȶ�
//		switch(ChargeDisDptr)     //ChargeStepBufͻ�� ������ChargeDisDptr ����ChargeDisable����
//		{
//			case LEDOFF:
//				LEDROFF;
//				LEDGOFF;
//				break;
//			case REDKEEPON:  //��Ƴ���
//					LEDRON;
//					LEDGOFF;								 
//				break;
//			case GREENFLASHON: //�̵���˸
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
//			case GREENKEEPON:   //�̵Ƴ���
//				LEDROFF;
//				LEDGON;
//				break;
//			case R_G_FLASH:     //���̽���
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
