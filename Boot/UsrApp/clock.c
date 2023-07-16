#include "clock.h"

extern uint8_t SRdkFrm_f10HzFlag;
/* 
 SystemFrequency / 1000 1ms �ж�һ�� * 
 SystemFrequency / 100000 10us �ж�һ�� * 
 SystemFrequency / 1000000 1us �ж�һ�� */
#define TICK_SLICE	1000
__IO uint32_t tick = 0; // tick����
__IO uint32_t overflow_flag = 0; // �����������ǣ�ͳ�����ʱ��

#ifdef USE_NOOS_DELAY
static uint8_t  fac_us = 0; // us��ʱ������			   
static uint16_t fac_ms = 0; // ms��ʱ������,��os��,����ÿ�����ĵ�ms��
#endif

// ��ʼ��tickʱ����ʹ��systick�����ж�������
void clock_init(void)
{
	/* set reload register */	
	SysTick->LOAD = ((SystemCoreClock / TICK_SLICE) & SysTick_LOAD_RELOAD_Msk) - 1; 
	
	/* set Priority for Cortex-M3 System Interrupts */
	NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
	
	/* Load the SysTick Counter Value */
	SysTick->VAL = 0;
	
	/* Enable SysTick IRQ and SysTick Timer */
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
				   SysTick_CTRL_TICKINT_Msk   | 
				   SysTick_CTRL_ENABLE_Msk;
}

// ��ȡ��ǰϵͳ���е�tick����λms
uint32_t GetTick(void)
{
	return tick;
}

// ��ȡ��������Ĵ���
uint32_t GetOverFlowTick(void)
{
	return overflow_flag;
}

#ifdef USE_NOOS_DELAY
// ��ʱnus
// nusΪҪ��ʱ��us��.	
// ע��:nus��ֵ,��Ҫ����798915us(���ֵ��2^24/fac_us@fac_us=21)
void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//ʱ�����	  		 
	SysTick->VAL=0x00;        				//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //�رռ�����
	SysTick->VAL =0X00;       				//��ռ����� 
}

// ��ʱnms
// ע��nms�ķ�Χ
// SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
// nms<=0xffffff*8*1000/SYSCLK
// SYSCLK��λΪHz,nms��λΪms
// ��168M������,nms<=798ms 
void delay_xms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms;			//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           			//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ���� 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;     		  		//��ռ�����	  	    
}

// ��ʱnms 
// nms:0~65535
void delay_ms(uint16_t nms)
{	 	 
	uint8_t repeat=nms/540;						//������540,�ǿ��ǵ�ĳЩ�ͻ����ܳ�Ƶʹ��,
											//���糬Ƶ��248M��ʱ��,delay_xms���ֻ����ʱ541ms������
	uint16_t remain=nms%540;
	while(repeat)
	{
		delay_xms(540);
		repeat--;
	}
	if(remain)delay_xms(remain);
} 
#endif

// systick�жϻص�������
void Tick_UpdateHandler(void)
{
	tick++;
	
	if(tick % 100 == 0)
		SRdkFrm_f10HzFlag = 1;
	
	if((tick & 0XFFFFFFFF) == 0)
	{
		overflow_flag++;
		tick = 0;
	}
}


