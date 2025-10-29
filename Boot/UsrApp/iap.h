#ifndef __IAP_H__
#define __IAP_H__

#include "main.h"

// 함수 유형의 매개변수 정의
typedef  void (*iapfun)(void);

// app 주소, bootloader 크기 16kB
#define FLASH_APP1_ADDR    (uint32_t)(FLASH_BASE | 0x00004000)

void iap_load_app(uint32_t appxaddr); // APP 프로그램으로 점프하여 실행
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t applen);	// 지정된 주소에서 시작하여 bin 파일 쓰기
void WFI_SET(void);		     // WFI 명령어 실행
void INTX_DISABLE(void);     // 모든 인터럽트 비활성화
void INTX_ENABLE(void);	     // 모든 인터럽트 활성화
void MSR_MSP(uint32_t addr); // 스택 포인터 주소 설정
void SoftReset(void);        // 소프트 리셋

#endif /* __IAP_H__ */
