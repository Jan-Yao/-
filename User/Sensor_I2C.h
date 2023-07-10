/**
   ******************************************************************************
   * @file		
   * @author	
   * @version	
   * @date		
   * @brief		
   ******************************************************************************
   */
	
#ifndef __Sensor_I2C_H_
#define __Sensor_I2C_H_    //检测是否已经编译过
	
#define Sensor_SetSDA()	GPIO_SetBits(Accelerometer_SDA_GBIO,Accelerometer_SDA_PIN)
#define Sensor_SetSCL()	GPIO_SetBits(Accelerometer_SCL_GBIO,Accelerometer_SCL_PIN)
#define Sensor_ClrSDA()	GPIO_ResetBits(Accelerometer_SDA_GBIO,Accelerometer_SDA_PIN)
#define Sensor_ClrSCL()	GPIO_ResetBits(Accelerometer_SCL_GBIO,Accelerometer_SCL_PIN)
#define Sensor_GetSDA()	GPIO_ReadInputDataBit(Accelerometer_SDA_GBIO,Accelerometer_SDA_PIN)

//#define SCLOut()	GPIOA->MODER |= GPIO_MODER_MODER4_0;
//#define SDAOut()	Accelerometer_SDA_GBIO->CRL&=0X0FFFFFFF;Accelerometer_SDA_GBIO->CRL|=3<<28;
//#define SDAIn()		Accelerometer_SDA_GBIO->CRL&=0X0FFFFFFF;Accelerometer_SDA_GBIO->CRL|=8<<28;

//#define GetINT2() GPIO_ReadPinInput(BOARD_INITPINS_FS8471_INT2_GPIO, BOARD_INITPINS_FS8471_INT2_GPIO_PIN)

// Accelerometer I2C address
#define Accelerometer_SLAVE_ADDR_W		0x3c 	// with pins SA0=0, SA1=0  详见本文档第1条
#define Accelerometer_SLAVE_ADDR_R		0x3d 	// with pins SA0=0, SA1=0   

// Accelerometer internal register addresses
#define Accelerometer_STATUS				0x00
#define Accelerometer_INT_SOURCE		0x0C    //中断源
#define Accelerometer_WHOAMI				0x0D    //设备识别
#define Accelerometer_XYZ_DATA_CFG          0x0E    //滤波器与量程
#define Accelerometer_A_FFMT_CFG            0x15    //自由落体/运动配置寄存器
#define Accelerometer_A_FFMT_SRC            0x16    //自由落体/运动数据锁存器
#define Accelerometer_A_FFMT_THS            0x17    //阈值配置
#define Accelerometer_A_FFMT_COUNT          0x18    //消抖时间配置
#define Accelerometer_ASLP_COUNT            0x29    //睡眠定时
#define Accelerometer_CTRL_REG1             0x2A    //模式降噪    ODR  
#define Accelerometer_CTRL_REG2             0x2B    //Sleep
#define Accelerometer_CTRL_REG4             0x2D    //中断使能寄存器
#define Accelerometer_CTRL_REG5             0x2E    //中断路由配置寄存器
#define Accelerometer_WHOAMI_VAL            0x6A


void Accelerometer_init(void);
void Accelerometer_Sleep(void);
void Sensor_ReadAccel(void);
void SetAngelZero(void);
void Limit(void);

#endif

