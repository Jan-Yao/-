/**
  ******************************************************************************
  * @file		
  * @author	
  * @version	
  * @date		
  * @brief		
  ******************************************************************************
  */

#include <stdio.h>
#include <math.h>
#include "Sensor_I2C.h"
#include "pin_mcu.h" 
#include "usart.h"
#include "Flash.h"

uint8_t  LD_Limit = 0;
uint16_t ReadAccelCount = 0;
uint16_t System1s = 0;
int32_t Angle = 0;
//int32_t Angle_Data = 0;
int32_t Accel_X = 0;
int32_t Accel_Y = 0;
int32_t Accel_Z = 0;

void Sensor_SCLOut(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_uInitStructure.GPIO_Pin = Accelerometer_SCL_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP; 
//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOA,&GPIO_uInitStructure);
}

void Sensor_SDAOut(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_uInitStructure.GPIO_Pin = Accelerometer_SDA_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP; 
//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOA,&GPIO_uInitStructure);
}

void Sensor_SDAIn(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_uInitStructure.GPIO_Pin = Accelerometer_SDA_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_uInitStructure.GPIO_OType = GPIO_OType_OD; 
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOA,&GPIO_uInitStructure);
}

/**
 * @brief	软件延时
 * @param	i,延时数
  * @retva	None
  */
void Sensor_I2CDelay(uint32_t i)
{
	i *= 2;
	for (; i > 0; i--)
  {

	}
}

/**
 * @brief	I2C接口初始化
  * @param	None
  * @retva	None
  */
void  Sensor_I2cInit (void)
{
	Sensor_SCLOut();
	Sensor_SDAOut();
	Sensor_SetSCL();
	Sensor_SetSDA();
}

/**
 * @brief	I2C起始时序
  * @param	None
  * @retva	None
  */
void Sensor_I2CStart(void)
{
	Sensor_SetSDA();
	Sensor_SetSCL();
	Sensor_I2CDelay(10);
	Sensor_ClrSDA();
	Sensor_I2CDelay(5);
	Sensor_ClrSCL();
}

/**
 * @brief	I2C结束时序
  * @param	None
  * @retva	None
  */
void Sensor_I2CStop(void)
{
	Sensor_ClrSCL();
	Sensor_ClrSDA();
	Sensor_I2CDelay(5);
	Sensor_SetSCL();
	Sensor_I2CDelay(10);
	Sensor_SetSDA();
}

/**
 * @brief	发送应答位
 * @param	Ack,应答状态
  * @retva	None
  */
void Sensor_I2CSendAck(uint8_t Ack)
{
	if(Ack)
  {
		Sensor_ClrSDA();
	}
    else
    {
		Sensor_SetSDA();
	}
	Sensor_I2CDelay(5);
	Sensor_SetSCL();
	Sensor_I2CDelay(5);
	Sensor_ClrSCL();

	Sensor_SetSDA();
}

/**
 * @brief	等待应答
  * @param	None
 * @retva	应答状态
  */
uint8_t Sensor_I2CWaitAck(void)
{
	uint8_t u8State;

	Sensor_SetSDA();
	Sensor_SDAIn();
	Sensor_SetSCL();
	Sensor_I2CDelay(5);
	if(!Sensor_GetSDA())
    {
		Sensor_ClrSCL();
		u8State = 1;
	} 
    else
    {
		Sensor_ClrSCL();
		Sensor_I2CStop();
		u8State = 0;
	}
	Sensor_SDAOut();
	return u8State;
}

/**
 * @brief	写一个字节数据
  * @param	Data
  * @retva	None
  */
void Sensor_I2CWriteByte(uint8_t Data)
{
	uint8_t i;
	Sensor_SDAOut();
	Sensor_ClrSCL();
	for(i = 0; i < 8; i++)
    {
		if(Data & 0x80)
        {
			Sensor_SetSDA();
		} 
        else
        {
			Sensor_ClrSDA();
		}
		Sensor_I2CDelay(5);
		Sensor_SetSCL();
		Data <<= 1;
		Sensor_I2CDelay(5);
		Sensor_ClrSCL();
	}
}


/**
 * @brief	读一个字节数据
  * @param	Data
  * @retva	None
  */
uint8_t  Sensor_I2CReadByte(void)   //读1字节数据
{
	uint8_t i,I_data;
	
	I_data = 0;
    Sensor_SetSDA();
	Sensor_SDAIn();
	for(i=0;i<8;i++)
	{
		Sensor_SetSCL();
        Sensor_I2CDelay(5);
        I_data = I_data<<1;
		if(Sensor_GetSDA() == 1)I_data = I_data + 1;
        Sensor_ClrSCL();
        Sensor_I2CDelay(5);
	}
    Sensor_SDAOut();
	return(I_data);
}

/**
 * @brief	从机的某一寄存器写数据
  * @param	Data
  * @retva	None
  */
void Sensor_WriteReg(uint8_t reg,uint8_t dat)  //给从机的某一寄存器写数据
{
	Sensor_I2CStart();//起始
	Sensor_I2CWriteByte(Accelerometer_SLAVE_ADDR_W); //器件地址，写
    Sensor_I2CWaitAck();
	Sensor_I2CWriteByte(reg); //写寄存器地址
    Sensor_I2CWaitAck();
	Sensor_I2CWriteByte(dat); //写数据
    Sensor_I2CWaitAck();
	Sensor_I2CStop();//停止
}

/**
 * @brief	从从机的某一寄存器读数据
  * @param	Data
  * @retva	None
  */
uint8_t Sensor_ReadReg(uint8_t reg)   //从从机的某一寄存器读数据
{
	uint8_t dat;

	Sensor_I2CStart();//起始
	Sensor_I2CWriteByte(Accelerometer_SLAVE_ADDR_W); //确定器件地址写命令
	Sensor_I2CWaitAck();
	Sensor_I2CWriteByte(reg); //写寄存器地址
	Sensor_I2CWaitAck();
	Sensor_I2CStart();//重复起始条件
	Sensor_I2CWriteByte(Accelerometer_SLAVE_ADDR_R); //确定器件地址读命令
  Sensor_I2CWaitAck();
	dat = Sensor_I2CReadByte();//读取最后一个字节数据
	Sensor_I2CSendAck(0);//非应答
	Sensor_I2CStop();//停止	
	return dat;
}

/**
 * @brief	初始化
  * @param	Data
  * @retva	None
  */
void Accelerometer_init(void)   //加速度计初始化设置
{
	uint8_t dat;
    
  Sensor_I2cInit();

	dat = Sensor_ReadReg(Accelerometer_WHOAMI);	//读取设备识别寄存器 寄存器地址为0             D，数据为    6A
	if(dat == Accelerometer_WHOAMI_VAL)
	{
	}
  Sensor_WriteReg(Accelerometer_CTRL_REG1,0x00);   //请看数据手册对寄存器的描述
	Sensor_WriteReg(Accelerometer_XYZ_DATA_CFG,0x00);  //[7-5]: 无意义  [4]: 0 禁用输出滤波器  [3-2]: 无意义  [1-0]: 00 ±0.244 mg/LSB
	Sensor_WriteReg(Accelerometer_CTRL_REG1,0x05); //[7-6]: 00  睡眠模式   [5-3]:    dr=100 50Hz(20ms) ODR    [2]: 1 降噪模式  [1]: 0  14    bit数据  [0]: 1 激活模式
}

/**
 * @brief	Sleep
  * @param	Data
  * @retva	None
  */
void Accelerometer_Sleep(void)   //加速度计初始化设置
{
	Sensor_WriteReg(Accelerometer_CTRL_REG1,0x00);
}

/**
 * @brief	读取加速度值
  * @param	Data
  * @retva	None
  */
void Sensor_ReadAccel(void)   //读取加速度值
{
	uint8_t i;
	uint8_t Buffer[7];
    
  int16_t AccelData[3];
	
	float Accel_1G = 0;
	float Angle_temp_Z = 0;
	float Angle_temp_Send = 0;
	
	Sensor_I2CStart();//起始
	Sensor_I2CWriteByte(Accelerometer_SLAVE_ADDR_W); //写入写命令
	Sensor_I2CWaitAck();
	Sensor_I2CWriteByte(Accelerometer_STATUS); //写寄存器地址
	Sensor_I2CWaitAck();
	Sensor_I2CStart();//重复起始条件
	Sensor_I2CWriteByte(Accelerometer_SLAVE_ADDR_R); //写入读命令
  Sensor_I2CWaitAck();
	for(i=0; i<6; i++)
	{
		Buffer[i] = Sensor_I2CReadByte();//读取一个字节数据
		Sensor_I2CSendAck(1);//应答
	}
	Buffer[6]= Sensor_I2CReadByte();//读取最后一个字节数据
	Sensor_I2CSendAck(1);//非应答
	Sensor_I2CStop();//停止        iic通讯读取加速度传感器中的三轴加速度值
	
	AccelData[0] = ((Buffer[1] << 8) | Buffer[2]);  //14位数据
  AccelData[0] >>= 2;          //X轴加速度值
	AccelData[1] = ((Buffer[3] << 8) | Buffer[4]);
	AccelData[1] >>= 2;          //Y轴加速度值
	AccelData[2] = ((Buffer[5] << 8) | Buffer[6]);
	AccelData[2] >>= 2;          //Z轴加速度值

  Accel_X += AccelData[0];
	Accel_Y += AccelData[1];
	Accel_Z += AccelData[2];
	
  ReadAccelCount++;
  if(ReadAccelCount >= 50)
  {
		Accel_X = Accel_X / ReadAccelCount;    //三轴加速度求平均值
		Accel_Y = Accel_Y / ReadAccelCount;
		Accel_Z = Accel_Z / ReadAccelCount;

		ReadAccelCount = 0;

		if(SysPar.SysStruct.AngelZero_Z != 0)
		{
			Accel_X -= SysPar.SysStruct.AngelZero_X;
	    Accel_Y -= SysPar.SysStruct.AngelZero_Y;
			Accel_Z -= SysPar.SysStruct.AngelZero_Z;
	    Accel_Z = 4096 + Accel_Z;
		}
		
		Accel_1G = sqrt(Accel_X * Accel_X + Accel_Y * Accel_Y);
		Accel_Z = sqrt(Accel_Z * Accel_Z);
//			Accel_1G -= SysPar.SysStruct.Accel_1G;
		Angle_temp_Z = atan(Accel_1G / Accel_Z) * 18000.0 / 3.1415926;     //弧度转化为角度
		Angle_temp_Send = Angle_temp_Z / 100;
		if(Red_Accel == 1)
		{
		  sprintf((char *)TxBuffer,"Accel_X = %d\r\n",Accel_X);
		  USARTSendData(TxBuffer);
		  sprintf((char *)TxBuffer,"Accel_Y = %d\r\n",Accel_Y);
		  USARTSendData(TxBuffer);
		  sprintf((char *)TxBuffer,"Accel_Z = %d\r\n",Accel_Z);
		  USARTSendData(TxBuffer);
		  sprintf((char *)TxBuffer,"Accel = %5.2f\r\n",Angle_temp_Send);
      USARTSendData(TxBuffer);
		}
	
		Accel_X = 0;
		Accel_Y = 0;
		Accel_Z = 0;
	
		Angle = (int32_t)Angle_temp_Z;
  }
}


void SetAngelZero(void)
{
  uint8_t i;
	uint8_t Buffer[7];
  uint16_t AccelCount_Zero;
  int16_t AccelData[3];
	
	int32_t Accel_X_Zero = 0;
  int32_t Accel_Y_Zero = 0;
  int32_t Accel_Z_Zero = 0;
	
//	float Accel_1G = 0;
//	float Angle_temp = 0;
  
  for(AccelCount_Zero = 0;AccelCount_Zero < 200;AccelCount_Zero++)
  {	
    Sensor_I2CStart();//起始
    Sensor_I2CWriteByte(Accelerometer_SLAVE_ADDR_W); //写入写命令
    Sensor_I2CWaitAck();
    Sensor_I2CWriteByte(Accelerometer_STATUS); //写寄存器地址
    Sensor_I2CWaitAck();
    Sensor_I2CStart();//重复起始条件
    Sensor_I2CWriteByte(Accelerometer_SLAVE_ADDR_R); //写入读命令
    Sensor_I2CWaitAck();
    for(i=0; i<6; i++)
    {
      Buffer[i] = Sensor_I2CReadByte();//读取一个字节数据
      Sensor_I2CSendAck(1);//应答
    }
    Buffer[6]= Sensor_I2CReadByte();//读取最后一个字节数据
    Sensor_I2CSendAck(1);//非应答
    Sensor_I2CStop();//停止        iic通讯读取加速度传感器中的三轴加速度值
    
    AccelData[0] = ((Buffer[1] << 8) | Buffer[2]);  //14位数据
    AccelData[0] >>= 2;          //X轴加速度值
    AccelData[1] = ((Buffer[3] << 8) | Buffer[4]);
    AccelData[1] >>= 2;          //Y轴加速度值
    AccelData[2] = ((Buffer[5] << 8) | Buffer[6]);
    AccelData[2] >>= 2;          //Z轴加速度值

      
    Accel_X_Zero += AccelData[0];
    Accel_Y_Zero += AccelData[1];
    Accel_Z_Zero += AccelData[2];
  }

  Accel_X_Zero = Accel_X_Zero / AccelCount_Zero;    //三轴加速度求平均值
  Accel_Y_Zero = Accel_Y_Zero / AccelCount_Zero;
  Accel_Z_Zero = Accel_Z_Zero / AccelCount_Zero;

	SysPar.SysStruct.AngelZero_X = (int32_t)Accel_X_Zero;
	SysPar.SysStruct.AngelZero_Y = (int32_t)Accel_Y_Zero;
	SysPar.SysStruct.AngelZero_Z = (int32_t)Accel_Z_Zero;

	sprintf((char *)TxBuffer,"AngelZero_X = %d\r\n",Accel_X_Zero);
	USARTSendData(TxBuffer);
	sprintf((char *)TxBuffer,"AngelZero_Y = %d\r\n",Accel_Y_Zero);
	USARTSendData(TxBuffer);
	sprintf((char *)TxBuffer,"AngelZero_Z = %d\r\n",Accel_Z_Zero);
	USARTSendData(TxBuffer);
}

void Limit(void)
{
	System1s++;
//	Sensor_ReadAccel();
	if(System1s >= 100)
	{
		System1s = 0;
		if(Angle >= 400)
		{
			LD_Limit = 1;
		}
		else if(LD_Limit == 1)
		{
			if(Angle <= 380)
			{
				LD_Limit = 0;
			}
		}
	}
}

