#ifndef __APPCFG_H__
#define __APPCFG_H__

#include "stdio.h"
#include "string.h"

// Ӧ�ù��ܺ�
#define DFS	50655

// �������
//#define   VERSION_SOFTDATA		20190904
#define VERSION_SOFTDATA_YEAR		0x23
#define VERSION_SOFTDATA_MONTH		0x07
#define VERSION_SOFTDATA_DAY		0x02

// Ӳ��ƽ̨��
#define STM32_F103	0x00000001
#define STM32_F407	0x00000002

#define PLATFORM_VERSION	STM32_F103

/* �����͹ر�Ӧ�ù��� */
#define	DISABLE_FUNC 0
#define	ENABLE_FUNC  1


#ifndef LED_ENABLE
	#define LED_ENABLE	(ENABLE_FUNC)
#endif

// ����TRACE
#define TRACE_DBG
// ���ڶ���
//#define USART_QUEUE
#ifdef USART_QUEUE
#define USART_QUEUE_TEST
#endif

// ����RX DMAģʽ����������֡�ж�
#define ENABLE_UART_RX_DMA
#define ENABLE_USART1_IRQ

// ���Ե�Ԫ����
//#define FLASH_WR_TEST
#define YMODEM_TEST

// ����bootloader
//#define BOOT_ENABLE
#ifdef	BOOT_ENABLE
	// 16KB��Ϊboot loader����ռ�
	#define LOADER_SIZE      (uint32_t)0x4000
	#define APP_BASE_ADDRESS (uint32_t)(0x08000000 | LOADER_SIZE)
#endif


#endif /* __APPCFG_H__ */
