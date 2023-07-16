#ifndef __FLASH_H__
#define __FLASH_H__

#include "main.h"
#include "AppCfg.h"
#include "iap.h"

#if (PLATFORM_VERSION == STM32_F103)
	// FLASH��ʼ��ַ
	#define STM32_FLASH_BASE	FLASH_BASE 	// STM32 FLASH����ʼ��ַ
	// FLASH ҳ��С
	#define STM32_PAGE_SIZE		0x400 // 1KB
	// ��ʼҳ
	#define STM32_PAGE_START	((FLASH_APP1_ADDR - FLASH_BASE)/1024)
	// ҳ����64,flash�ܴ�С64*1KB
	#define MAX_PAGE_COUNT		64
	#define STM32_APP_PAGE_COUNT	(MAX_PAGE_COUNT - STM32_PAGE_START)
#elif(PLATFORM_VERSION == STM32_F407)
	// FLASH��ʼ��ַ
	#define STM32_FLASH_BASE FLASH_BASE 	// STM32 FLASH����ʼ��ַ
	// FLASH ��������ʼ��ַ
	#define STM32_SECTOR0	0x08000000
#endif

// public:
typedef enum
{
	FLASH_ERROR = 0,
	FLASH_ERASE_ERROR,
	FLASH_WRITE_ERROR,
	FLASH_READ_ERROR,
	FLASH_COMPLETE,
	FLASH_LOCK,
	FLASH_UNLOCK,
} FlashStatus;

// public:
FlashStatus FLASH_Erase(uint32_t addr, uint32_t size); // ��ָ����ַ��ʼ����ָ������
uint32_t FLASH_ReadWord(uint32_t addr);	// ������
FlashStatus FLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite); // ��ָ����ַ��ʼд��ָ�����ȵ�����
void FLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead); // ��ָ����ַ��ʼ����ָ�����ȵ�����

#endif /* __FLASH_H__ */
