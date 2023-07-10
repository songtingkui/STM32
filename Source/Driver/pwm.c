/*********************************************************************************************************
* ģ�����ƣ�pwm.c
* 
* ժ    Ҫ��4·PWM���ο��ƽӿ�
* 
* ��ǰ�汾��1.0.0
* 
* ��    �ߣ�ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* ������ڣ�2021��05��11�� 
* 
* ��    �ݣ�1��PWMʱ�ӳ�ʼ��
*			2��PWM���ƽӿ�
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

#include "pwm.h"


/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
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
* �������ƣ�void PWM_Init(void)
* 
* �������ܣ�PWMʱ�Ӽ��ӿ�IO��ʼ����ʹ��Time2
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
void PWM_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
	
	// Clock
	RCC_AHBPeriphClockCmd(LED_PWM_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(PWM_TIME_CLK, ENABLE);
	
	// PWM Channel 1, 2, 3, 4
	GPIO_InitStructure.GPIO_Pin  = LED_PWM_PIN1 | LED_PWM_PIN2 | LED_PWM_PIN3 | LED_PWM_PIN4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Disable;
	GPIO_Init(LED_PWM_PORT, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(LED_PWM_PORT, GPIO_PinSource1, GPIO_AF_4);	
	GPIO_PinAFConfig(LED_PWM_PORT, GPIO_PinSource2, GPIO_AF_4);	
	GPIO_PinAFConfig(LED_PWM_PORT, GPIO_PinSource3, GPIO_AF_4);	
	GPIO_PinAFConfig(LED_PWM_PORT, GPIO_PinSource4, GPIO_AF_4);
	
	// TIM2: counter clock at 10 MHz = PWM_Pulse(1K) * PWM_Frequency(10K)
	// TIM2CLK = HCLK = SystemCoreClock��Prescaler = (TIM2CLK / TIM2 counter clock) - 1
	TIM_TimeBaseStructure.TIM_Period = PWM_PULSE - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / (PWM_PULSE * PWM_FREQ)) - 1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
    // ͨ��1��2��3��4��PWMģʽ����: TIMx_CNT < TIMx_CCR1 ��ͨ��Ϊ�͵�ƽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OutputNState = 0;
	TIM_OCInitStructure.TIM_OCNPolarity = 0;
	TIM_OCInitStructure.TIM_OCIdleState = 0;
	TIM_OCInitStructure.TIM_OCNIdleState = 0;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    
    /* ͨ��2 */
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    
    /* ͨ��3 */
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	
	/* ͨ��4 */
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	
	/* TIM2 counter enable */
	TIM_Cmd(TIM2, ENABLE);
	
	/* TIM2 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM2, ENABLE);	
}
