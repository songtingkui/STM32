/*********************************************************************************************************
* ģ�����ƣ�ADC.c
* 
* ժ    Ҫ��ADC˫ͨ��ת����PD6��PC4��
* 
* ��ǰ�汾��1.0.0
* 
* ��    �ߣ�
* 
* ������ڣ�2021��12��15�� 
* 
* ��    �ݣ�
*			
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
				���޸ĵĿ⺯��
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
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "hk32f030m.h"
#include "blinker.h"
#include "ADC.h"
#include "pwm.h"
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

/*********************************************************************************************************
*                                              ȫ�ֱ���
*********************************************************************************************************/
uint16_t 	ADCConvertedValue;
float 		ADCConvertedVoltage1;
float 		ADCConvertedVoltage2;
//uint16_t 	value1;
//uint16_t 	value2;
//char 		arVolage1[7];
//char 		arVolage2[7];
/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
* �������ƣ�void ADC_Initialize(void)
* 
* �������ܣ�ADC�ӿڳ�ʼ��
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
void ADC_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  //ģ�� 
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;  //����
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Disable;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_7);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_7);
	//ADC Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, ENABLE);
	
	ADC_ClockModeConfig(ADC1,ADC_ClockMode_SynClkDiv4);   //��
	
	ADC_DeInit(ADC1);   
	ADC_StructInit(&ADC_InitStructure);   //Ĭ������
	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;     //����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  //+	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;//*/ADC_ScanDirection_Backward;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	//3 NVIC
	NVIC_InitTypeDef NVIC_InitStructure;	
	
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;         		            //ѡ��TIM3��IRQͨ��
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;             	            //IRQͨ�����ȼ�=0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         		       	    //ʹ��IRQͨ��
	NVIC_Init(&NVIC_InitStructure); 	
	
	/* ADC1 regular channels configuration  ADC1����ͨ������ */ 
	ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_2 , ADC_SampleTime_239_5Cycles);
	//У��ADC
	ADC_GetCalibrationFactor(ADC1);

	/* Enable ADC1 */
	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);			//�жϿ���
	ADC_Cmd(ADC1, ENABLE);
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
	ADC_StartOfConversion(ADC1);
}


/*********************************************************************************************************
* �������ƣ�void ADC_Initialize(void)
* 
* �������ܣ�ADC�ж�
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
void ADC1_IRQHandler()
{

	static uint32_t adcCount = 1;
	
	if(ADC_GetITStatus(ADC1,ADC_IT_EOC) == SET) 
	{

		if (adcCount == 1)
		{	//����
			ADCConvertedValue = ADC_GetConversionValue(ADC1);
			ADCConvertedVoltage1 = (ADCConvertedValue * 3.30)/4095;				//����ADCת����ѹֵ
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
		{	//����
			ADCConvertedValue = ADC_GetConversionValue(ADC1);				
			ADCConvertedVoltage2 = (ADCConvertedValue * 3.30)/4095;				//����ADCת����ѹֵ
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
* �������ƣ�void Photosensitive(void)
* 
* �������ܣ�ADC�ж�
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




