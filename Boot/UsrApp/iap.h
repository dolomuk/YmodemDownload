#ifndef __IAP_H__
#define __IAP_H__

#include "main.h"

// ����һ���������͵Ĳ���. 
typedef  void (*iapfun)(void);

// app��ַ��bootloader��С16kB
#define FLASH_APP1_ADDR    (uint32_t)(FLASH_BASE | 0x00004000)

void iap_load_app(uint32_t appxaddr); // ��ת��APP����ִ��
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t applen);	// ��ָ����ַ��ʼ,д��bin
void WFI_SET(void);		     // ִ��WFIָ��
void INTX_DISABLE(void);     // �ر������ж�
void INTX_ENABLE(void);	     // ���������ж�
void MSR_MSP(uint32_t addr); // ���ö�ջ��ַ
void SoftReset(void);        // ��λ

#endif /* __IAP_H__ */
