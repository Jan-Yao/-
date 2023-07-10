/**
  ******************************************************************************
  * @file		flash.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  */
#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdbool.h>
#include "stm32f0xx.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SYSPAR_LEN        7
#define SYSPAR_BYTES_LEN	28

#define SYS_PAR_LOCATION	0x08007C00  //数据存放地址0x08003C00
#define FLASH_SECTOR_SIZE	0x400


typedef struct 
{
	uint32_t InitFlag;
  uint32_t A;
  int32_t  Bat_V_Error;
  int32_t  AngelZero_X;
	int32_t  AngelZero_Y;
	int32_t  AngelZero_Z;
	uint8_t Reserves[4];
} __attribute__ ((packed)) SYSPARA_STRUC;

union SysPar_u 
{
	SYSPARA_STRUC SysStruct;
	uint32_t p[SYSPAR_LEN];
};

/*******************************************************************************
 * 全局变量声明
 ******************************************************************************/
extern union SysPar_u SysPar;

uint8_t Pflash_Init(void);
uint8_t SaveInformation(void);

#endif
