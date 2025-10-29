#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f103xb.h"
#include "flash.h"
#include "usart.h"


#define FLASH_TYPEPROGRAM_BYTE        0x00000000U  /*!< Program byte (8-bit) at a specified address           */
#define FLASH_TYPEPROGRAM_HALFWORD    0x00000001U  /*!< Program a half-word (16-bit) at a specified address   */
#define FLASH_TYPEPROGRAM_WORD        0x00000002U  /*!< Program a word (32-bit) at a specified address        */


// Flash ?? ??
FlashStatus FLASH_Unlock(void)
{
	FlashStatus status = FLASH_COMPLETE;

	if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
	{
		WRITE_REG(FLASH->KEYR, FLASH_KEY1);
		WRITE_REG(FLASH->KEYR, FLASH_KEY2);

		if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
		{
			status = FLASH_ERROR;
		}
	}
	return status;
}

// Flash ??
FlashStatus FLASH_Lock(void)
{
	FlashStatus status = FLASH_COMPLETE;

	SET_BIT(FLASH->CR, FLASH_CR_LOCK);
	if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 1U)
	{
		status = FLASH_ERROR;
	}
	return status;
}

// ?? ??? Flash ?? ?? ??
FlashStatus HAL_FLASH_OB_Unlock(void)
{
	FlashStatus status = FLASH_UNLOCK;

	if(READ_BIT(FLASH->CR, FLASH_CR_OPTWRE) != 0U)
	{
		WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY1);
		WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY2);
		
		if(READ_BIT(FLASH->CR, FLASH_CR_OPTWRE) == 0U)
		{
			status = FLASH_LOCK;
		}
	}
	return status;
}

// ?? ??? Flash ?? ??
FlashStatus HAL_FLASH_OB_Lock(void)
{
	FlashStatus status = FLASH_LOCK;

	SET_BIT(FLASH->CR, FLASH_CR_OPTWRE);

	if (READ_BIT(FLASH->CR, FLASH_CR_OPTWRE) == 0U)
	{
		status = FLASH_UNLOCK;
	}

	return status;
}

// ��ָ����ַ��ʼ����ָ�����ȣ�������1024��������
FlashStatus FLASH_Erase(uint32_t addr, uint32_t size)
{
	#define PAGE_ERASE_SUCCESS	0xFFFFFFFF // �ж��Ƿ�����ɹ�
	#define TIMEOUT_COUNT		0xFFFFFFFF // ��ʱ�ȴ�
	__IO uint32_t nIndex = 0, timeout = 0;
	FlashStatus status = FLASH_COMPLETE;
	
	// step0��У���ַ�Ƿ�Ϊҳ����������У���ַ�Ƿ�ΪС�ڻ�ַ���Ƿ�Խ��
	if(((addr + size) % STM32_PAGE_SIZE != 0) || (addr < STM32_FLASH_BASE) 
		|| (addr >= FLASH_BASE + (MAX_PAGE_COUNT * STM32_PAGE_SIZE)))
		return FLASH_ERROR;
	
	FLASH_Unlock();
	
	// step1���ȴ�flash�ɲ���
	timeout = 0;
	while(READ_BIT(FLASH->SR, FLASH_SR_BSY))
	{
		if(timeout++ >= TIMEOUT_COUNT) 
			break;
	}
	
	// step2������flash
	for(nIndex = addr; nIndex < addr + size; nIndex += STM32_PAGE_SIZE)
	{
		SET_BIT(FLASH->CR, FLASH_CR_PER); // ��ҳ����
		WRITE_REG(FLASH->AR, FLASH_AR_FAR & nIndex); // ������ҳ�׵�ַ
		SET_BIT(FLASH->CR, FLASH_CR_STRT); // ��ʼ����
		
		// �ȴ�����
		timeout = 0;
		while(READ_BIT(FLASH->SR, FLASH_SR_BSY))
		{
			if(timeout++ >= TIMEOUT_COUNT) 
			{
				FLASH_Lock();
				return FLASH_ERASE_ERROR;
			}
		}
		
		// ��ȡ��֤�Ƿ�����ɹ�
		if(*(uint32_t *)nIndex != PAGE_ERASE_SUCCESS)
		{
			status = FLASH_ERASE_ERROR;
			//p_err("FLASH_Erase error at 0x%x", nIndex);
			break;
		}
	}
	// ������
	SET_BIT(FLASH->SR, FLASH_SR_EOP);
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	
	FLASH_Lock();
	
	return status;
}

// ��ָ����ַ��ʼд��
FlashStatus FLASH_Program(uint8_t program_type, uint32_t addr, uint32_t val)
{
	#define TIMEOUT_COUNT		0xFFFFFFFF // ��ʱ�ȴ�
	__IO uint32_t timeout = 0;
	FlashStatus status = FLASH_COMPLETE;

	// step0������
	FLASH_Unlock();
	
	// step1���ȴ�flash�ɲ���
	timeout = 0;
	while(READ_BIT(FLASH->SR, FLASH_SR_BSY))
	{
		if(timeout++ >= TIMEOUT_COUNT) 
			break;
	}
	if(RESET == READ_BIT(FLASH->CR, FLASH_CR_PG))
		SET_BIT(FLASH->CR, FLASH_CR_PG); // ���ñ��
	
	switch(program_type)
	{
	#if 0
	case FLASH_TYPEPROGRAM_BYTE: // д���ֽڣ�8bit��
		*(__IO uint8_t *)addr = (uint8_t)(val);
		break;
	#endif
	case FLASH_TYPEPROGRAM_HALFWORD: // д����֣�16bit��
		*(__IO uint16_t *)addr = (uint16_t)(val);
		break;
		
	case FLASH_TYPEPROGRAM_WORD: // д���֣�32bit��
		*(__IO uint16_t *)addr = (uint16_t)(val);
		__ISB();
		*(__IO uint16_t *)(addr+2) = (uint16_t)(val>>16);
		break;
	
	default:
		status = FLASH_WRITE_ERROR;
		break;
	}
	
	if(status != FLASH_WRITE_ERROR)
	{
		// �ȴ�д��
		timeout = 0;
		while(READ_BIT(FLASH->SR, FLASH_SR_BSY))
		{
			if(timeout++ >= TIMEOUT_COUNT) 
			{
				FLASH_Lock();
				return FLASH_ERROR;
			}
		}
		// ��ȡ��֤�Ƿ�д��ɹ�
		if(*(uint32_t *)addr != val)
		{
			status = FLASH_WRITE_ERROR;
			//p_err("FLASH_Write error at 0x%x", addr);
		}
	}
	// ������
	if(READ_BIT(FLASH->SR, FLASH_SR_EOP))
		SET_BIT(FLASH->SR, FLASH_SR_EOP);
	
	if(READ_BIT(FLASH->CR, FLASH_CR_PG))
		CLEAR_BIT(FLASH->CR, FLASH_CR_PG); 
	
	FLASH_Lock(); // ����
	
	return status;
}

// ������
uint32_t FLASH_ReadWord(uint32_t addr)
{
	return *(__IO uint32_t*)addr;
}


// ��ָ����ַ��ʼд��ָ�����ȵ�����
FlashStatus FLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t NumToWrite)
{
	uint32_t i = 0;
	FlashStatus status = FLASH_COMPLETE;
	
	/* Step0��У���ַ�Ƿ�ΪС�ڻ�ַ���Ƿ�Խ�� */
	if((WriteAddr < STM32_FLASH_BASE) || (WriteAddr >= FLASH_BASE + (MAX_PAGE_COUNT * STM32_PAGE_SIZE)))
		return FLASH_ERROR;
	
	/* Step1��д��ǰ�Ƚ��в������� */
	if(NumToWrite < STM32_PAGE_SIZE)
		FLASH_Erase(WriteAddr, STM32_PAGE_SIZE); // С��1ҳ��ֱ�Ӱ�1ҳ����
	else if(NumToWrite % STM32_PAGE_SIZE == 0)
		FLASH_Erase(WriteAddr, NumToWrite); // ��������ֱ�Ӱ�ҳ��������
	else
		FLASH_Erase(WriteAddr, (NumToWrite/STM32_PAGE_SIZE)+1); // ���������������1ҳ
	
	/* Step2������ */
	status = FLASH_Unlock();
	if(status != FLASH_COMPLETE)
		return FLASH_ERROR;
	
	/* Step3��д��flash */
	while(i < NumToWrite)
	{
		status = FLASH_Program(FLASH_TYPEPROGRAM_WORD, WriteAddr, *(pBuffer + i));
		if(status != FLASH_COMPLETE)
		{
			FLASH_Lock();
			return FLASH_WRITE_ERROR;
		}
		WriteAddr += 4;
		i++; // ����ƫ��1��
	}
	
	/* Step4������������ */
	status = FLASH_Lock();
	if(status != FLASH_COMPLETE)
		return FLASH_ERROR;
	
	return status;
}

// ��ָ����ַ��ʼ����ָ�����ȵ�����
void FLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead)
{
	uint32_t dataIndex;
	
	// У���ַ�Ƿ�ΪС�ڻ�ַ���Ƿ�Խ��
	if((ReadAddr < STM32_FLASH_BASE) || (ReadAddr >= FLASH_BASE + (MAX_PAGE_COUNT * STM32_PAGE_SIZE)))
		return;
	
    for (dataIndex = 0; dataIndex < NumToRead; dataIndex++)
    {
        pBuffer[dataIndex] = FLASH_ReadWord(ReadAddr + dataIndex * 4);
    }
}
