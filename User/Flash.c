/**
  ******************************************************************************
  * @file		flash.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "Flash.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
union SysPar_u SysPar;

/*******************************************************************************
* Function Name  : Flash_eeprom_WriteWithPacked
* Description    : Write a group of datas to flash.
* Input          : buff:pointer of first data, length: write length
* Output         : 
* Return         : true/false
*******************************************************************************/
bool WriteFlash(void)
{
    uint8_t i;
    
    FLASH_Unlock();    

    /*Clear all flags*/
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );
    
    /*Erase first . Do not rember.*/
    if(FLASH_COMPLETE != FLASH_ErasePage(SYS_PAR_LOCATION))//�����ȴ�
    {
        return false;
    }
    
    /*Write head*/
		for (i = 0; i < SYSPAR_LEN; i++) 
		{
			FLASH_ProgramWord(SYS_PAR_LOCATION+(i*4), SysPar.p[i]);
		}  
    
    FLASH_Lock();
    return true;    
}

/**
  * @brief	���������Flash.
  * @param	None
  * @retva	None
  */
uint8_t SaveInformation(void) 
{
	FLASH_Status result;

	/* ����0x08003C00----0x08003FFF���� */
	result = FLASH_ErasePage(SYS_PAR_LOCATION);
	if (FLASH_COMPLETE != result) 
	{
		return 202;//Flash����ʧ��
	}
	
	if(WriteFlash() == false)
	{
		return 204;//Flashд��ʧ��
	}
	return 0;
//	/* ������֤ */
//	result = FLASH_VerifyErase(&s_flashDriver, SYS_PAR_LOCATION, FLASH_SECTOR_SIZE, kFLASH_MarginValueUser);
//	if (kStatus_FLASH_Success != result)
//	{
//		return 203;//Flash������֤ʧ��
//	}
//	/* д������ */
//	result = FLASH_Program(&s_flashDriver, SYS_PAR_LOCATION, SysPar.p, SYSPAR_BYTES_LEN);
//	if (kStatus_FLASH_Success != result) 
//	{
//		return 204;//Flashд��ʧ��
//	}
//	/* д����֤ */
//	result = FLASH_VerifyProgram(&s_flashDriver, SYS_PAR_LOCATION, SYSPAR_BYTES_LEN, SysPar.p, kFLASH_MarginValueUser,
//	                             &failAddr, &failDat);
//	if (kStatus_FLASH_Success != result)
//	{
//		return 205;//Flashд����֤ʧ��
//	}
//	return 0;
}

/**
  * @brief	flash��ʼ��.
  * @param	None
  * @retva	None
  */
uint8_t Pflash_Init(void) 
{
	uint8_t result;    /* Return code from each flash driver function */
	uint32_t i;
	uint32_t dataRead;


//	/* Clean up Flash driver Structure*/
//	memset(&s_flashDriver, 0, sizeof(flash_config_t));

//	/* Setup flash driver structure for device and initialize variables. */
//	result = FLASH_Init(&s_flashDriver);
//	if (kStatus_FLASH_Success != result) {
//		return 200;//Flash��ʼʧ��
//	}

//	/* Check security status. */
//	result = FLASH_GetSecurityState(&s_flashDriver, &securityStatus);
//	if (kStatus_FLASH_Success != result) {
//		return 201;//Flash��ⰲȫ״̬ʧ��
//	}

	/* Test pflash basic opeation only if flash is unsecure. */
//	if (kFLASH_SecurityStateNotSecure == securityStatus) 
//	{
		//ϵͳ������ʼ��
		dataRead = *(uint32_t*)SYS_PAR_LOCATION;
		if(dataRead != 0x777777) 
		{ //д��Ĭ��ֵ
			SysPar.SysStruct.InitFlag = 0x777777;
			SysPar.SysStruct.A = 0;
			SysPar.SysStruct.Bat_V_Error = 0;
			SysPar.SysStruct.AngelZero_X = 0;
			SysPar.SysStruct.AngelZero_Y = 0;
			SysPar.SysStruct.AngelZero_Z = 0;
			result = SaveInformation();
			if(result != 0)return result;
		}
		else 
		{ //����������ݶ������ڴ�
			for (i = 0; i < SYSPAR_LEN; i++) 
			{
				SysPar.p[i] = *(uint32_t*)(SYS_PAR_LOCATION + (i * 4));
			}
		}
//	} 
//	else 
//	{
//		return 201;//Flash�������̲��ܰ�ȫִ��
//	}
	return 0;
}
