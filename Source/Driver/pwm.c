/*********************************************************************************************************
* 模块名称：pwm.c
* 
* 摘    要：4路PWM波形控制接口
* 
* 当前版本：1.0.0
* 
* 作    者：ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* 完成日期：2021年05月11日 
* 
* 内    容：1、PWM时钟初始化
*			2、PWM控制接口
*			
* 注    意：                                                                 
**********************************************************************************************************
* 取代版本：
* 
* 作    者： 
* 
* 完成日期：
* 
* 修改内容：
* 
* 修改文件：
*********************************************************************************************************/


/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "hk32f030m.h"

#include "pwm.h"


/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
 
/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：
* 函数功能：
* 输入参数：
* 输出参数：
* 返 回 值：
* 创建日期：2018年01月01日
* 注    意：
* 修改记录：
*********************************************************************************************************/


/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：void PWM_Init(void)
* 
* 函数功能：PWM时钟及接口IO初始化，使用Time2
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：
* 修改记录：
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
	// TIM2CLK = HCLK = SystemCoreClock，Prescaler = (TIM2CLK / TIM2 counter clock) - 1
	TIM_TimeBaseStructure.TIM_Period = PWM_PULSE - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / (PWM_PULSE * PWM_FREQ)) - 1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
    // 通道1，2，3，4的PWM模式设置: TIMx_CNT < TIMx_CCR1 则通道为低电平
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OutputNState = 0;
	TIM_OCInitStructure.TIM_OCNPolarity = 0;
	TIM_OCInitStructure.TIM_OCIdleState = 0;
	TIM_OCInitStructure.TIM_OCNIdleState = 0;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    
    /* 通道2 */
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    
    /* 通道3 */
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	
	/* 通道4 */
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	
	/* TIM2 counter enable */
	TIM_Cmd(TIM2, ENABLE);
	
	/* TIM2 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM2, ENABLE);	
}
