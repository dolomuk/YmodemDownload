#ifndef __IAP_H__
#define __IAP_H__

#include "main.h"

// �Լ� ������ �Ű����� ����
typedef  void (*iapfun)(void);

// app �ּ�, bootloader ũ�� 16kB
#define FLASH_APP1_ADDR    (uint32_t)(FLASH_BASE | 0x00004000)

void iap_load_app(uint32_t appxaddr); // APP ���α׷����� �����Ͽ� ����
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t applen);	// ������ �ּҿ��� �����Ͽ� bin ���� ����
void WFI_SET(void);		     // WFI ��ɾ� ����
void INTX_DISABLE(void);     // ��� ���ͷ�Ʈ ��Ȱ��ȭ
void INTX_ENABLE(void);	     // ��� ���ͷ�Ʈ Ȱ��ȭ
void MSR_MSP(uint32_t addr); // ���� ������ �ּ� ����
void SoftReset(void);        // ����Ʈ ����

#endif /* __IAP_H__ */
