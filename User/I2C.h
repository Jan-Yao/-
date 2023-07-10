/**
   ******************************************************************************
   * @file		FXLS8471Q.h
   * @author	
   * @version	
   * @date		
   * @brief		
   ******************************************************************************
   */
	
#ifndef __I2C_H_
#define __I2C_H_    //检测是否已经编译过
	
#include "stm32f0xx.h"

#define SetSDA()	GPIO_SetBits(Charging_SDA_GBIO,Charging_SDA_PIN)
#define SetSCL()	GPIO_SetBits(Charging_SCL_GBIO,Charging_SCL_PIN)
#define ClrSDA()	GPIO_ResetBits(Charging_SDA_GBIO,Charging_SDA_PIN)
#define ClrSCL()	GPIO_ResetBits(Charging_SCL_GBIO,Charging_SCL_PIN)
#define GetSDA()	GPIO_ReadInputDataBit(Charging_SDA_GBIO,Charging_SDA_PIN)

#define Charging_ADDR_W		0xD6 	
#define Charging_ADDR_R		0xD7 	

#define REG00		0x00   //输入控制寄存器
#define REG01		0x01   //上电配置寄存器
#define REG02		0x02   //充电电流控制寄存器
#define REG03		0x03   //预充电/终止电流控制寄存器
#define REG04		0x04   //充电电压控制寄存器
#define REG05		0x05   //充电终止/定时器控制寄存器
#define REG06		0x06   //热调节控制寄存器
#define REG07		0x07   //其他操作寄存器
#define REG08		0x08   //系统状态寄存器
#define REG09		0x09   //故障寄存器
#define REG0A		0x0A   //供应商/部件/修订状态寄存器

extern uint8_t  BqData[];

void I2cInit (void);
void WriteReg(uint8_t reg,uint8_t dat);
uint8_t ReadReg(uint8_t reg);

#endif

