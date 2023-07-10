/**
  ******************************************************************************
  * @file		
  * @author	
  * @version	
  * @date		
  * @brief		
  ******************************************************************************
  */

#include <math.h>
#include "I2C.h"
#include "pin_mcu.h" 

uint8_t  BqData[]={0X0E,0,0,0,0,0,0,0,0,0,0};

void SCLOut(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	
	GPIO_uInitStructure.GPIO_Pin = Charging_SCL_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_OD; 
//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
}

void SDAOut(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	
	GPIO_uInitStructure.GPIO_Pin = Charging_SDA_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_OD; 
//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
}

void SDAIn(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	
	GPIO_uInitStructure.GPIO_Pin = Charging_SDA_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_OD; 
//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
}

/**
 * @brief	软件延时
 * @param	i,延时数
  * @retva	None
  */
void I2CDelay(uint32_t i)
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
void  I2cInit (void)
{
	SCLOut();
	SDAOut();
	SetSCL();
	SetSDA();
}

/**
 * @brief	I2C起始时序
  * @param	None
  * @retva	None
  */
void I2CStart(void)
{
	SetSDA();
	SetSCL();
	I2CDelay(10);
	ClrSDA();
	I2CDelay(5);
	ClrSCL();
}

/**
 * @brief	I2C结束时序
  * @param	None
  * @retva	None
  */
void I2CStop(void)
{
	ClrSCL();
	ClrSDA();
	I2CDelay(5);
	SetSCL();
	I2CDelay(10);
	SetSDA();
}

/**
 * @brief	发送应答位
 * @param	Ack,应答状态
  * @retva	None
  */
void I2CSendAck(uint8_t Ack)
{
	if(Ack)
    {
		ClrSDA();
	}
    else
    {
		SetSDA();
	}
	I2CDelay(5);
	SetSCL();
	I2CDelay(5);
	ClrSCL();

	SetSDA();
}

/**
 * @brief	等待应答
  * @param	None
 * @retva	应答状态
  */
uint8_t I2CWaitAck(void)
{
	uint8_t u8State;

	SetSDA();
	SDAIn();
	SetSCL();
	I2CDelay(5);
	if(!GetSDA())
    {
		ClrSCL();
		u8State = 1;
	} 
    else
    {
		ClrSCL();
		I2CStop();
		u8State = 0;
	}
	SDAOut();
	return u8State;
}

/**
 * @brief	写一个字节数据
  * @param	Data
  * @retva	None
  */
void I2CWriteByte(uint8_t Data)
{
	uint8_t i;
	SDAOut();
	ClrSCL();
	for(i = 0; i < 8; i++)
    {
		if(Data & 0x80)
        {
			SetSDA();
		} 
        else
        {
			ClrSDA();
		}
		I2CDelay(5);
		SetSCL();
		Data <<= 1;
		I2CDelay(5);
		ClrSCL();
	}
}


/**
 * @brief	读一个字节数据
  * @param	Data
  * @retva	None
  */
uint8_t  I2CReadByte(void)   //读1字节数据
{
	uint8_t i,I_data;
	
	I_data = 0;
    SetSDA();
	SDAIn();
	for(i=0;i<8;i++)
	{
		SetSCL();
        I2CDelay(5);
        I_data = I_data<<1;
		if(GetSDA() == 1)I_data = I_data + 1;
        ClrSCL();
        I2CDelay(5);
	}
    SDAOut();
	return(I_data);
}

/**
 * @brief	从机的某一寄存器写数据
  * @param	Data
  * @retva	None
  */
void WriteReg(uint8_t reg,uint8_t dat)  //给从机的某一寄存器写数据
{
	I2CStart();//起始
	I2CWriteByte(Charging_ADDR_W); //器件地址，写
    I2CWaitAck();
	I2CWriteByte(reg); //写寄存器地址
    I2CWaitAck();
	I2CWriteByte(dat); //写数据
    I2CWaitAck();
	I2CStop();//停止
}

/**
 * @brief	从从机的某一寄存器读数据
  * @param	Data
  * @retva	None
  */
uint8_t ReadReg(uint8_t reg)   //从从机的某一寄存器读数据
{
	uint8_t dat;

	I2CStart();//起始
	I2CWriteByte(Charging_ADDR_W); //确定器件地址写命令
	I2CWaitAck();
	I2CWriteByte(reg); //写寄存器地址
	I2CWaitAck();
	I2CStart();//重复起始条件
	I2CWriteByte(Charging_ADDR_R); //确定器件地址读命令
  I2CWaitAck();
	dat = I2CReadByte();//读取最后一个字节数据
	I2CSendAck(0);//非应答
	I2CStop();//停止	
	return dat;
}



