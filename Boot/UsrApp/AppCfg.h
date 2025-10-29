#ifndef __APPCFG_H__
#define __APPCFG_H__

#include "stdio.h"
#include "string.h"

// ?????? ?? ??
#define DFS	50655

// ????? ??
//#define   VERSION_SOFTDATA		20190904
#define VERSION_SOFTDATA_YEAR		0x23
#define VERSION_SOFTDATA_MONTH		0x07
#define VERSION_SOFTDATA_DAY		0x02

// ???? ??? ??
#define STM32_F103	0x00000001
#define STM32_F407	0x00000002

#define PLATFORM_VERSION	STM32_F103

/* ?????? ?? ??? ? ???? */
#define	DISABLE_FUNC 0
#define	ENABLE_FUNC  1


#ifndef LED_ENABLE
	#define LED_ENABLE	(ENABLE_FUNC)
#endif

// TRACE ???
#define TRACE_DBG
// UART ?
//#define USART_QUEUE
#ifdef USART_QUEUE
#define USART_QUEUE_TEST
#endif

// RX DMA ?? ???, ?? ?? ?? ???
#define ENABLE_UART_RX_DMA
#define ENABLE_USART1_IRQ

// ??? ?? ??
//#define FLASH_WR_TEST
#define YMODEM_TEST

// ???? ???
//#define BOOT_ENABLE
#ifdef	BOOT_ENABLE
	// 16KB? ???? ???? ???? ??
	#define LOADER_SIZE      (uint32_t)0x4000
	#define APP_BASE_ADDRESS (uint32_t)(0x08000000 | LOADER_SIZE)
#endif


#endif /* __APPCFG_H__ */
