/*********************************************************************************************************
* ģ�����ƣ�key.c
* 
* ժ    Ҫ�����ض����������ƽӿڣ�PA1������Ч��
* 
* ��ǰ�汾��1.0.0
* 
* ��    �ߣ�ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* ������ڣ�2021��05��11�� 
* 
* ��    �ݣ�1���ӿڳ�ʼ�� - ����
*			2������ʶ�� - PA1������Ч
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

#include "key.h"
#include "delay.h"
#include "blinker.h"
/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define KEY_JIT_TIME		50									// ����ȥ������ʱʱ��(ͬ״̬����ʱ��)ms
#define KEY_LONG_TIME		1500								// ������ʶ��ʱ��ms
#define KEY_SHORT_TIME		100								// �̰���ʶ��ʱ��ms
#define KEY_PWRON_STATUS	Bit_SET								// �����ϵ�Ĭ��״̬

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static uint8_t  KeyShortStatus;									// �а������������� & �ͷţ���1 - ������0 - û��
static uint8_t  KeyLongStatus;									// �г�����������1 - ������0 - û��

extern uint8_t MusicVolumeUp[4];		//������
extern uint8_t MusicVolumeDown[4];		//������
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
* �������ƣ�void Key_Init(void)
* 
* �������ܣ����������ӿڳ�ʼ��
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
void Key_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// Clock
	RCC_AHBPeriphClockCmd(MAIN_KEY_CLK, ENABLE);
	
	// GPIO
    GPIO_InitStructure.GPIO_Pin  = MAIN_KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Enable;
    GPIO_Init(MAIN_KEY_PORT, &GPIO_InitStructure);
	
}

/*********************************************************************************************************
* �������ƣ�void ScanKeyboard1(void)
* 
* �������ܣ�ɨ�谴��״̬�仯
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺��Ҫʵʱ���ã�����ʶ���˰����仯	!!!!!!!!!!!
* 
* �޸ļ�¼��
*********************************************************************************************************/
void ScanKeyboard1(void)
{
	static uint32_t Keysteps = 1;	
	static uint32_t KeyTick  = 1;
	switch(Keysteps)
	{
		case 1:
			//��鰴�·�
			if(0 == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
			{
				KeyTick = g_u32_1msTick;
				Keysteps = 2;
			}
		
		break;
			
		case 2:
			//ȥ�����ȴ�
			if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_JIT_TIME)
			{
				Keysteps = 3;
			}
		
		
		break;
		
		case 3:
			//�ٴ�ȷ���Ƿ񰴼�
			if(0 == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
			{
				Keysteps = 4;
			}
			else
			{
				Keysteps = 1;
			}
		break;
		
		case 4:
			
		if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_SHORT_TIME)
		{
			//�̰�������
			GPIO_SetBits(GPIOC, GPIO_Pin_7);	
			
			
			
			Keysteps = 5;
		}
			
		break;
		
		case 5:
		if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_LONG_TIME)
		{
			//����������
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
			KeyLongStatus = 1;
			
			Keysteps = 6;
		}
		else if(0 != GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
		{
			Keysteps = 1;	
			KeyShortStatus = 1; 
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);			
		}	
		break;
		
		case 6:
		
		//�ȴ������ͷ�
		if(0 != GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
		{
			Keysteps = 1;	
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		}	
		break;
	}
	
}	

/*********************************************************************************************************
* �������ƣ�void ScanKeyboard1(void)
* 
* �������ܣ�ɨ�谴��״̬�仯
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺��Ҫʵʱ���ã�����ʶ���˰����仯	!!!!!!!!!!!
* 
* �޸ļ�¼��
*********************************************************************************************************/
void ScanKeyboard2(void)
{
	static uint32_t Keysteps = 1;	
	static uint32_t KeyTick  = 1;
	switch(Keysteps)
	{
		case 1:
			//��鰴�·�
			if(0 == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))
			{
				KeyTick = g_u32_1msTick;
				Keysteps = 2;
			}
		
		break;
			
		case 2:
			//ȥ�����ȴ�
			if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_JIT_TIME)
			{
				Keysteps = 3;
			}
		
		
		break;
		
		case 3:
			//�ٴ�ȷ���Ƿ񰴼�
			if(0 == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))
			{
				Keysteps = 4;
			}
			else
			{
				Keysteps = 1;
			}
		break;
		
		case 4:
			
		if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_SHORT_TIME)
		{
			//�̰�������
			GPIO_SetBits(GPIOC, GPIO_Pin_7);
			KeyShortStatus = 2;    
			
			
			Keysteps = 5;
		}
			
		break;
		
		case 5:
		if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_LONG_TIME)
		{
			//����������
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
			KeyLongStatus = 2;
			
			Keysteps = 6;
		}
		else if(0 != GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))
		{
			Keysteps = 1;	
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		}	
		break;
		
		case 6:
		
		//�ȴ������ͷ�
		if(0 != GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))
		{
			Keysteps = 1;	
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		}	
		break;
	}
	
}

/*********************************************************************************************************
* �������ƣ�void sysKeyProcess(void)
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
void sysKeyProcess(void)
{
	// KEY_PA1���̰�����
	if (1 == KeyShortStatus)
	{
		KeyShortStatus = 0;
		
		// ����״̬�µ�������->����
		BlinkAdjustBright();		
	}
	
	// KEY_PA1��ⳤ������
	if (1 == KeyLongStatus)
	{
		KeyLongStatus = 0;
		
		// �л��ƿ���״̬-> �� ��
		BlinkToggleLed();	
	}
	
	// KEY_PA2���̰�����
	if (2 == KeyShortStatus)
	{
		KeyShortStatus = 0;
		
		// ����״̬�µ�������->����
		BlinkAdjustBright2();		
	}
	
	// KEY_PA2������ⳤ������
	if (2 == KeyLongStatus)
	{
		KeyLongStatus = 0;
		
		// �л��ʵƵ�״̬
		 BlinkColorchose();
		
	}
}
