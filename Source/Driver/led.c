/*********************************************************************************************************
* ģ�����ƣ�led.c
* 
* ժ    Ҫ������LEDָʾ�ƿ��ƽӿڣ�PA2��������
* 
* ��ǰ�汾��1.0.0
* 
* ��    �ߣ�ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* ������ڣ�2021��05��11�� 
* 
* ��    �ݣ�1���ӿڳ�ʼ��
*			2���������
*			
* ע    �⣺                                                                 
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

#include "led.h"
#include "delay.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
uint32_t LedStep = 1;
uint32_t LedTick = 0;


/*********************************************************************************************************
*                                              ȫ�ֱ���
*********************************************************************************************************/

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
* �������ƣ�void Led_Init(void)
* 
* �������ܣ�LEDָʾ�ƽӿڳ�ʼ��
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void Led_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Clock
	RCC_AHBPeriphClockCmd(MAIN_LED_CLK, ENABLE);
	
	// GPIO
    GPIO_InitStructure.GPIO_Pin  = MAIN_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Disable;
    GPIO_Init(MAIN_LED_PORT, &GPIO_InitStructure);
	
	// ��̬���ƣ�����
    GPIO_ResetBits(MAIN_LED_PORT, MAIN_LED_PIN);
}

/*********************************************************************************************************
* �������ƣ�void Led_On(void)
* 
* �������ܣ�LEDָʾ����
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/

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
void Ledshow(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	//PC3	
	switch (LedStep)
		{
			case 1:
				// 1.��
				GPIO_ResetBits(GPIOx, GPIO_Pin);
			
				LedTick = g_u32_1msTick;
				LedStep = 2;			
			break;
				
			case 2:
				// 2.�ȴ�		
				if ((uint32_t)(g_u32_1msTick - LedTick) >= 500 )
				{	
					LedStep = 3;
				}
				break;
			
			case 3:
				// 3.��
				GPIO_SetBits(GPIOx, GPIO_Pin);
				LedTick = g_u32_1msTick;
				LedStep = 4;			
			
			break;
			
			case 4:
				// 4.�ȴ�
				if ((uint32_t)(g_u32_1msTick - LedTick) >= 500)
				{		
					LedStep = 1;
				}
			break;
		}		
}



/*********************************************************************************************************
* �������ƣ�uint32_t Led_IsOn(void)
* 
* �������ܣ�ָʾ������
* 
* ���������None
* ���������None
* 
* �� �� ֵ��0 - ��1 - ��
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
