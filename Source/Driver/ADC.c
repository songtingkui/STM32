/*********************************************************************************************************
* 模块名称：ADC.c
* 
* 摘    要：ADC双通道转换（PD6，PC4）
* 
* 当前版本：1.0.0
* 
* 作    者：
* 
* 完成日期：2021年12月15日 
* 
* 内    容：
*			
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
				需修改的库函数
void ADC_StartOfConversion(ADC_TypeDef* ADCx)
{
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  
  ADCx->CR |= (uint32_t)ADC_CR_ADSTART;
}



void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct)
{
  uint32_t tmpreg = 0;
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG_EDGE(ADC_InitStruct->ADC_ExternalTrigConvEdge));
  assert_param(IS_ADC_EXTERNAL_TRIG_CONV(ADC_InitStruct->ADC_ExternalTrigConv));
  assert_param(IS_ADC_DATA_ALIGN(ADC_InitStruct->ADC_DataAlign));
  assert_param(IS_ADC_SCAN_DIRECTION(ADC_InitStruct->ADC_ScanDirection)); 

  ADCx->CHSELR = 0;
}


*********************************************************************************************************/


/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "hk32f030m.h"
#include "blinker.h"
#include "ADC.h"
#include "pwm.h"
#include "delay.h"

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
*                                              全局变量
*********************************************************************************************************/
uint16_t 	ADCConvertedValue;
float 		ADCConvertedVoltage1;
float 		ADCConvertedVoltage2;
//uint16_t 	value1;
//uint16_t 	value2;
//char 		arVolage1[7];
//char 		arVolage2[7];
/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/

/*********************************************************************************************************
* 函数名称：void ADC_Initialize(void)
* 
* 函数功能：ADC接口初始化
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
void ADC_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  //模拟 
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;  //不拉
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Disable;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_7);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_7);
	//ADC Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, ENABLE);
	
	ADC_ClockModeConfig(ADC1,ADC_ClockMode_SynClkDiv4);   //加
	
	ADC_DeInit(ADC1);   
	ADC_StructInit(&ADC_InitStructure);   //默认配置
	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;     //连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  //+	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;//*/ADC_ScanDirection_Backward;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	//3 NVIC
	NVIC_InitTypeDef NVIC_InitStructure;	
	
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;         		            //选择TIM3的IRQ通道
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;             	            //IRQ通道优先级=0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         		       	    //使能IRQ通道
	NVIC_Init(&NVIC_InitStructure); 	
	
	/* ADC1 regular channels configuration  ADC1常规通道配置 */ 
	ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_2 , ADC_SampleTime_239_5Cycles);
	//校正ADC
	ADC_GetCalibrationFactor(ADC1);

	/* Enable ADC1 */
	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);			//中断开启
	ADC_Cmd(ADC1, ENABLE);
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
	ADC_StartOfConversion(ADC1);
}


/*********************************************************************************************************
* 函数名称：void ADC_Initialize(void)
* 
* 函数功能：ADC中断
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
void ADC1_IRQHandler()
{

	static uint32_t adcCount = 1;
	
	if(ADC_GetITStatus(ADC1,ADC_IT_EOC) == SET) 
	{

		if (adcCount == 1)
		{	//光敏
			ADCConvertedValue = ADC_GetConversionValue(ADC1);
			ADCConvertedVoltage1 = (ADCConvertedValue * 3.30)/4095;				//计算ADC转换电压值
//			value1 = (uint16_t)(ADCConvertedVoltage * 100);
			
//			arVolage1[0] = (value1 / 100) + 0x30;
//			arVolage1[1] = '.';
//			arVolage1[2] = (value1 % 100) / 10 + 0x30;
//			arVolage1[3] = (value1 % 10)  + 0x30;
//			arVolage1[4] = '\r';
//			arVolage1[5] = '\n';
			
			adcCount = 2;
		}
		else 
		{	//声控
			ADCConvertedValue = ADC_GetConversionValue(ADC1);				
			ADCConvertedVoltage2 = (ADCConvertedValue * 3.30)/4095;				//计算ADC转换电压值
//			value2 = (uint16_t)(ADCConvertedVoltage * 100);
			
//			arVolage2[0] = (value2 / 100) + 0x30;
//			arVolage2[1] = '.';
//			arVolage2[2] = (value2 % 100) / 10 + 0x30;
//			arVolage2[3] = (value2 % 10)  + 0x30;
//			arVolage2[4] = '\r';
//			arVolage2[5] = '\n';
			
			adcCount = 1;
			ADC_StartOfConversion(ADC1);				
		}
	}
}


/*********************************************************************************************************
* 函数名称：void Photosensitive(void)
* 
* 函数功能：ADC中断
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
void Photosensitive(void)
{
	static uint32_t Photosense_Tick;
	if (0 == g_strBlinkerData.led_switch)
	{
		
		if (2 < ADCConvertedVoltage2)
		{
			Photosense_Tick = g_u32_1msTick;
			PWM_Drive_PD4(100);
		}
		
		if(2.5 < ADCConvertedVoltage1)
		{
			Photosense_Tick = g_u32_1msTick;
			PWM_Drive_PD4(100);
		}
		if ((uint32_t)(g_u32_1msTick - Photosense_Tick) >= 1000)
		{
			PWM_Drive_PD4(0);
		}
	}
		

}




