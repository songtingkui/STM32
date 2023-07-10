/*********************************************************************************************************
* ģ�����ƣ�softUart.c
* 
* ժ    Ҫ��IOģ�⴮�ڿ�
* 
* ��ǰ�汾��1.0.0
* 
* ��    �ߣ�ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* ������ڣ�2021��05��11�� 
* 
* ��    �ݣ�1��softUart��ʼ��
*			2��softUart�����շ�
*			
* ע    �⣺1�������ڿ���������շ�ģʽ��������˫��ͨ�ţ��շ���ʱ
*			2�������ڼ�ʹ��Systick��ʱ������ʱ�������ã�
*			3������ʹ��Timer1���������ã������ⲿ�жϿ������޸�IO����Ҫ�����ж�
*			4���������ݿ����ⲿ��ѯ �� �жϽ���
*			5�����������ý�����Ӳ�����ڵ�3�����ң���ֹ�����ڵ����շ�ʱ�����Ӳ������©������
*                                                                 
**********************************************************************************************************
* ȡ���汾��
* 
* ��    �ߣ� 
* 
* ������ڣ�
* 
* �޸����ݣ�
* 
* �޸��ļ���
*********************************************************************************************************/


/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "hk32f030m.h"

#include "softUart.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define SOFT_TX_PORT		GPIOC												// ����TX�˿�
#define SOFT_TX_PIN			GPIO_Pin_5
#define SOFT_TX_CLK			RCC_AHBPeriph_GPIOC

#define SOFT_RX_PORT		GPIOC												// ����RX�˿�: �޸ı��˿ں�ע���޸��ⲿ�жϴ���
#define SOFT_RX_PIN			GPIO_Pin_6
#define SOFT_RX_CLK			RCC_AHBPeriph_GPIOC

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static uint8_t  s_u8SoftUartRecvData, s_u8SoftUartHaveData;
static uint32_t s_u32SoftUartBaudTick;

//extern uint16_t comCount;														// �жϽ��ղ���
//extern uint8_t comBuffer[256];
extern uint32_t g_u8ComRecvSoft;
extern uint32_t g_u32ComRecv_mark;
/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�
* �������ܣ�
* ���������
* ���������
* �� �� ֵ��
* �������ڣ�2018��01��01��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
								    


/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�void softUart_init(uint32_t baudRate)
* 
* �������ܣ���ʼ������IO �� ���ն�ʱ�� �� �ⲿ�ж�
* 
* ���������baudRate = ���ڲ����ʣ����57600�������̶���8, n, 1
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void softUart_init(uint32_t baudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	// �����ʻ���Ϊ��ʱʱ��������ʱ�ӣ�����Ƶ��
	s_u8SoftUartHaveData  = 0;
	s_u8SoftUartRecvData  = 0;
	s_u32SoftUartBaudTick = SystemCoreClock / baudRate - 1;
	
	// TX
	RCC_AHBPeriphClockCmd(SOFT_TX_CLK, ENABLE);
    GPIO_SetBits(SOFT_TX_PORT, SOFT_TX_PIN);					//PC5
	
    GPIO_InitStructure.GPIO_Pin  = SOFT_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
    GPIO_Init(SOFT_TX_PORT, &GPIO_InitStructure);
	
	// RX
	RCC_AHBPeriphClockCmd(SOFT_RX_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = SOFT_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Enable;
	GPIO_Init(SOFT_RX_PORT, &GPIO_InitStructure);

	// ����bit����ʱ��: TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = s_u32SoftUartBaudTick;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	// RX: EXTI
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);
	EXTI_ClearITPendingBit(EXTI_Line6);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*********************************************************************************************************
* �������ƣ�void EXTI3_IRQHandler(void)
* 
* �������ܣ�����RX: �ⲿ�жϳ���
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺1��ʹ�ñ���ȫ�ֱ�����¼��������
*			2���жϽ������ݣ��ڡ��û��жϽ��մ���
*			3�������������գ���Ӱ�촮�ڷ���
* 
* �޸ļ�¼��
*********************************************************************************************************/
void EXTI6_IRQHandler(void)
{
	uint32_t i, dat;
  
	__disable_irq();
	if (EXTI_GetFlagStatus(EXTI_Line6) != RESET)
	{
		// TIM1 enable counter
		TIM_SetCounter(TIM1, 0);
		TIM_ClearFlag(TIM1, TIM_FLAG_Update);
		TIM_Cmd(TIM1, ENABLE);	  
	
		// 8bit����
		dat = 0;
		for (i = 0; i < 8; i++)
		{
			// �ȴ�bit���ʱ��
			while (RESET == TIM_GetFlagStatus(TIM1, TIM_FLAG_Update));
			TIM_ClearFlag(TIM1, TIM_FLAG_Update);
			
			// ����1bit��D7 -> D0
			dat >>= 1;
			if (RESET != GPIO_ReadInputDataBit(SOFT_RX_PORT, SOFT_RX_PIN))
			{
				dat |= 0x80;
			}
		}

		// 8bit������ϣ�����ж�λ
		TIM_Cmd(TIM1, DISABLE);	  
		EXTI_ClearITPendingBit(EXTI_Line6);
		
		// ������ݼ���־
		s_u8SoftUartRecvData = dat;
		s_u8SoftUartHaveData = 1;

		// �û��жϽ��մ���
		g_u8ComRecvSoft = s_u8SoftUartRecvData;
		g_u32ComRecv_mark = 1;
//		comBuffer[comCount] = s_u8SoftUartRecvData;
//		comCount++;
	}
	__enable_irq();
}

/*********************************************************************************************************
* �������ƣ�void softUart_SendChar(uint8_t ch)
* 
* �������ܣ��������ݷ��ͣ�8bit��������
* 
* ���������ch = Ҫ���������
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺�����������ͣ�ʹ��SysTick��ʱ��������Ƶ
* 
* �޸ļ�¼��
*********************************************************************************************************/
void softUart_SendChar(uint8_t ch)
{
	uint32_t i, sysTickCtrlBak;
	
	// ���Systick������־
	sysTickCtrlBak= SysTick->CTRL;

	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;	
	SysTick->VAL  = 0;
	SysTick->LOAD = s_u32SoftUartBaudTick;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;	
	
	// ��ʼλ
	__disable_irq();
    GPIO_ResetBits(SOFT_TX_PORT, SOFT_TX_PIN);

	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL &= (~SysTick_CTRL_COUNTFLAG_Msk);
	
	// 8bit����, D0 -> D7
	for (i = 0; i < 8; i++)
	{
		if (0 != (ch & 0x01))
		{
			GPIO_SetBits(SOFT_TX_PORT, SOFT_TX_PIN);
		}
		else
		{		
			GPIO_ResetBits(SOFT_TX_PORT, SOFT_TX_PIN);
		}
		ch >>= 1;

		while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
		SysTick->CTRL &= (~SysTick_CTRL_COUNTFLAG_Msk);
	}
	
	// ֹͣλ
    GPIO_SetBits(SOFT_TX_PORT, SOFT_TX_PIN);

	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL &= (~SysTick_CTRL_COUNTFLAG_Msk);

	// �رռ�����
	__enable_irq();
	SysTick->CTRL = sysTickCtrlBak;
}

/*********************************************************************************************************
* �������ƣ�uint32_t softUart_CheckRecv(void)
* 
* �������ܣ���ѯ�Ƿ��н�������
* 
* ���������None
* ���������None
* �� �� ֵ��0 - �ޣ� ���� - ��
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺���������ݱ�־��������ʱ�Զ�����
* 
* �޸ļ�¼��
*********************************************************************************************************/
uint32_t softUart_CheckRecv(void)
{
	return s_u8SoftUartHaveData;
}

/*********************************************************************************************************
* �������ƣ�uint32_t softUart_GetChar(void)
* 
* �������ܣ���ȡ���ڽ�������
* 
* ���������None
* ���������None
* �� �� ֵ��-1 = ���������ݣ����� - ���ν��յ���Ч����
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* 
* �޸ļ�¼���������Ȳ�ѯ���ձ�־
* 
*********************************************************************************************************/
uint32_t softUart_GetChar(void)
{
	// ���������ݱ�־
	if (s_u8SoftUartHaveData)
	{
		// �Զ������־
		s_u8SoftUartHaveData = 0;

		return s_u8SoftUartRecvData;
	} 
	else
	{
		// ��Ч����
		return -1UL;
	}
}

/////////////////////////////////////////////////////////

void TIM6_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;          		   	        //����TIM_TimeBaseInitTypeDef�ṹ����
	NVIC_InitTypeDef NVIC_InitStructure;                                        //����NVIC_InitTypeDef �����ṹ����

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); 				        //ʹ��TIM3ʱ��

	// ��ʱ����������
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;                      		    //�����Զ����ؼ���=1199+1=1200

	TIM_TimeBaseStructure.TIM_Prescaler = 32 - 1;                	 		    //��ʱ����ʱ��Ƶ��=TIM_Prescaler+1=2��
	TIM_TimeBaseStructure.TIM_ClockDivision = 1;               			        //���벶��ʱ�˲��õĲ���,������һ��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  		        //���ϼ���
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);              		        //����ָ���Ĳ�����ʼ

	// TIM6 �ж�Ƕ������

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;         		            //ѡ��TIM3��IRQͨ��
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;             	            //IRQͨ�����ȼ�=0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         		       	    //ʹ��IRQͨ��
	NVIC_Init(&NVIC_InitStructure);                                             //���Բ���������

	TIM_ClearFlag(TIM6, TIM_FLAG_Update);                        	            //�����־λ������һ�����жϺ����������ж�
	TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);                    		        //ʹ��TIM3����ж�
	
	TIM_Cmd(TIM6, ENABLE);                                                      //ʹ��TIM3 ���ж�

}
//////////////////////////////////////////


///////////////////////////////////////////////

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
void LED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;						//��������
	
	//Clocksʹ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);    		
	
	// GPIO��PA2
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Enable;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_2);						//��	
//////////////////////////////////////////////////////////////////////////
}


/*********************************************************************************************************
* �������ƣ�void Ledshow(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
* 
* �������ܣ�LEDָʾ����˸
* 
* ���������GPIO_TypeDef* GPIOx �˿�ABCD
*			uint16_t GPIO_Pin	�˿�1-7
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
//uint32_t LedStep = 1;
//uint32_t LedTick = 0;

//void Ledshow(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
//{
//	//PC3	
//	switch (LedStep)
//		{
//			case 1:
//				// 1.��
//				GPIO_ResetBits(GPIOx, GPIO_Pin);
//			
//				LedTick = g_u32_1msTick;
//				LedStep = 2;			
//			break;
//				
//			case 2:
//				// 2.�ȴ�		
//				if ((uint32_t)(g_u32_1msTick - LedTick) >= 500 )
//				{	
//					LedStep = 3;
//				}
//				break;
//			
//			case 3:
//				// 3.��
//				GPIO_SetBits(GPIOx, GPIO_Pin);
//				LedTick = g_u32_1msTick;
//				LedStep = 4;			
//			
//			break;
//			
//			case 4:
//				// 4.�ȴ�
//				if ((uint32_t)(g_u32_1msTick - LedTick) >= 500)
//				{		
//					LedStep = 1;
//				}
//			break;
//		}		
//}











