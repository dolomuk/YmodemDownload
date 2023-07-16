/*******************************************************************************
* �ļ���          : iap.c
* ����            : veis
* ʱ��		      : 07/06/2023
* �ļ�����        : IAP����
********************************************************************************/
#include "iap.h"
#include "flash.h"
#include "usart.h"

iapfun jump2app; 

#define FIFO_BASE			0x400
#define IAP_BUFFER_SIZE		256
uint32_t iapbuf[IAP_BUFFER_SIZE] __attribute__((at(SRAM_BASE | FIFO_BASE))); // 1K�ֽڻ���
//uint32_t iapbuf[512]; // 2K�ֽڻ���

// ��λ
void SoftReset(void)
{
	NVIC_SystemReset();
}

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//���������ж�
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(uint32_t addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}


// ��ת��Ӧ�ó����
// appxaddr:�û�������ʼ��ַ.
void iap_load_app(uint32_t appxaddr)
{
	INTX_DISABLE();
	if(((*(__IO uint32_t*)appxaddr)& 0x2FFE0000) == SRAM_BASE)	// ���ջ����ַ�Ƿ�Ϸ�
	{ 
		jump2app = (iapfun)*(__IO uint32_t*)(appxaddr + 4);		// �û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(__IO uint32_t*)appxaddr);					    // ��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									            // ��ת��APP
	}
	else
	{
		p_err("jump to app failed!");
		p_err("wait to software reset...");
		SoftReset();
	}
}

// appxaddr:Ӧ�ó������ʼ��ַ
// appbuf:Ӧ�ó���CODE.
// appsize:Ӧ�ó����С(�ֽ�).
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize)
{
	uint32_t t;
	uint16_t count = 0;
	uint32_t temp;
	uint8_t *dfu = appbuf;
	
	for(t = 0; t < appsize; t += 4)
	{						   
		temp =(uint32_t)(dfu[3] << 24);   
		temp |=(uint32_t)(dfu[2] << 16);    
		temp |=(uint32_t)(dfu[1] << 8);
		temp |=(uint32_t)(dfu[0]);	  
		dfu  += 4;//ƫ��4���ֽ�
		iapbuf[count++] = temp;	    
		if(count == IAP_BUFFER_SIZE)
		{
			count = 0; 
			FLASH_Write(appxaddr, iapbuf, 512);
			appxaddr += IAP_BUFFER_SIZE*4; // ƫ��2048  512*4=2048
		}
	} 
	if(count)
	{
		FLASH_Write(appxaddr, iapbuf, count); // ������һЩ�����ֽ�д��ȥ. 
	}		
}
