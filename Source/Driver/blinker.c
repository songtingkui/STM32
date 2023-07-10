/*********************************************************************************************************
* ģ�����ƣ�blinker.c
* 
* ժ    Ҫ����ƿƼ��ӿ�Э��ֽ�
* 
* ��ǰ�汾��1.0.0
* 
* ��    �ߣ�ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* ������ڣ�2021��05��11�� 
* 
* ��    �ݣ�
*			
* ע    �⣺1����֧��4�ֽӿ�Э��
*			2��ʹ��ȫ�ֻ��壬���봮�����ݰ��������������                                                               
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
#include <string.h>

#include "blinker.h"
#include "uart.h"
#include "pwm.h"
#include "03T.h"
#include "delay.h"
#include "eeprom.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define EEPROM_ADR_WWBRIGHT		0				// �������ȴ洢λ��
#define LED_ADJUST_STEP			25				// LED�����������Ȳ�����
#define LED_BRIGHT_SAVE_TIME	4000			// LED���ȵ����󣬱���ʱ��ms�����4,194MS

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ȫ�ֱ���
*********************************************************************************************************/
BLINKER_DataDef	g_strBlinkerData;				// LED����״̬����
uint32_t Ledchose = 0;
/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
 
/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void PWM_CloseBright(void);				// �ر�����
static void PWM_CloseColor(void);				// �رղʵ�
static void PWM_CloseAll(void);					// PWM���ȫ���ر�

static void PWM_SetBright(uint32_t brightness);	// �����������
static void PWM_SetColor(uint32_t r_value, uint32_t g_value, uint32_t b_value, uint32_t bright_value);	// �����߲����

static void AckBrightData(void);				// ����������������
static void AckColorData(void);					// �����ʵ���������
static void AckHeartBeat(void);					// ����������
static void AckSwitchOn(void);					// ��������on����
static void AckSwitchOff(void);					// ��������off����


/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
* �������ƣ�void PWM_CloseBright(void)
* 
* �������ܣ��ر�����
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
static void PWM_CloseBright(void)
{
	g_strBlinkerData.wwBright = 0;
	PWM_Drive_PD4(0);
}

/*********************************************************************************************************
* �������ƣ�void PWM_CloseColor(void)
* 
* �������ܣ��رղʵ�
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
static void PWM_CloseColor(void)
{
	g_strBlinkerData.b_value  = 0;
	g_strBlinkerData.r_value  = 0;
	g_strBlinkerData.g_value  = 0;
	PWM_Drive_PD3(0);
	PWM_Drive_PD2(0);
	PWM_Drive_PD1(0);
}

/*********************************************************************************************************
* �������ƣ�void PWM_CloseAll(void)
* 
* �������ܣ�PWM���ȫ���ر�
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
static void PWM_CloseAll(void)
{
	g_strBlinkerData.b_value  = 0;
	g_strBlinkerData.r_value  = 0;
	g_strBlinkerData.g_value  = 0;
	g_strBlinkerData.wwBright = 0;
	PWM_Drive_PD4(0);
	PWM_Drive_PD3(0);
	PWM_Drive_PD2(0);
	PWM_Drive_PD1(0);
}

/*********************************************************************************************************
* �������ƣ�void PWM_SetBright(uint32_t brightness)
* 
* �������ܣ���������PWM���
* 
* ���������brightness = �������ȱ���ֵ(0 ~ 255)
* 
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void PWM_SetBright(uint32_t brightness)
{
	PWM_Drive_PD4((brightness * WW_PWM_PULSE) / LED_BRIGH_MAX);	
}

/*********************************************************************************************************
* �������ƣ�void PWM_SetColor(uint32_t r_value, uint32_t g_value, uint32_t b_value, uint32_t bright_value)
* 
* �������ܣ������߲�PWM���
* 
* ���������r_value, g_value, b_value = �졢�̡��������ֵ(0 ~ 255)
*           bright_value = ���ȱ���ֵ(0 ~ 255)
* 
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void PWM_SetColor(uint32_t r_value, uint32_t g_value, uint32_t b_value, uint32_t bright_value)
{	
	PWM_Drive_PD1((b_value * B_PWM_PULSE * bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
	PWM_Drive_PD2((r_value * R_PWM_PULSE * bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
	PWM_Drive_PD3((g_value * G_PWM_PULSE * bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
}


/*********************************************************************************************************
* �������ƣ�static void AckBrightData(void)
* �������ܣ�����������������
* ���������
* ���������
* �� �� ֵ��
* �������ڣ�2018��01��01��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
static void AckBrightData(void)
{
	char	 sendBuf[64];
	int32_t  sendCount;

	// ������ֵ��{"ran-ww":{"val":100}}
	sendCount = sprintf(sendBuf, "{\"ran-ww\":{\"val\":%u}}\r\n", g_strBlinkerData.wwBright);

	if (sendCount > 0)
	{
		UART_SendStringLen(sendBuf, (uint32_t)sendCount);
	}
}

/*********************************************************************************************************
* �������ƣ�static void AckColorData(void)
* �������ܣ������ʵ���������
* ���������
* ���������
* �� �� ֵ��
* �������ڣ�2018��01��01��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
static void AckColorData(void)
{
	char	 sendBuf[64];
	int32_t  sendCount;
	
	// �߲ʵƹ�: {"col-led":[R,G,B,Brightness]}
	sendCount = sprintf(sendBuf, "{\"col-led\":[%u,%u,%u,%u]}\r\n", g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);

	if (sendCount > 0)
	{
		UART_SendStringLen(sendBuf, (uint32_t)sendCount);
	}
}

/*********************************************************************************************************
* �������ƣ�static void AckSwitchOn(void)
* �������ܣ���������on����
* ���������
* ���������
* �� �� ֵ��
* �������ڣ�2018��01��01��
* 
* ע    �⣺������Ϣ����ʹ��UTF-8���룬ʹ��\x�����notepad++ת������
* 
* �޸ļ�¼��
*********************************************************************************************************/
static void AckSwitchOn(void)
{
	// ����״̬: {"btn-abc":{"swi":"on","tex":"�ѿ���"}}
	UART_SendStringLen("{\"btn-abc\":{\"swi\":\"on\",\"tex\":\"\xe5\xb7\xb2\xe5\xbc\x80\xe7\x81\xaf\"}}\r\n", sizeof("{\"btn-abc\":{\"swi\":\"on\",\"tex\":\"\xe5\xb7\xb2\xe5\xbc\x80\xe7\x81\xaf\"}}\r\n") - 1);
}

/*********************************************************************************************************
* �������ƣ�static void AckSwitchOff(void)
* �������ܣ���������off����
* ���������
* ���������
* �� �� ֵ��
* �������ڣ�2018��01��01��
* 
* ע    �⣺������Ϣ����ʹ��UTF-8���룬ʹ��\x�����notepad++ת������
* 
* �޸ļ�¼��
*********************************************************************************************************/
static void AckSwitchOff(void)
{
	// ����״̬: {"btn-abc":{"swi":"off","tex":"�ѹص�"}}  
	UART_SendStringLen("{\"btn-abc\":{\"swi\":\"off\",\"tex\":\"\xe5\xb7\xb2\xe5\x85\xb3\xe7\x81\xaf\"}}\r\n", sizeof("{\"btn-abc\":{\"swi\":\"off\",\"tex\":\"\xe5\xb7\xb2\xe5\x85\xb3\xe7\x81\xaf\"}}\r\n") - 1);
}

/*********************************************************************************************************
* �������ƣ�static void AckHeartBeat(void)
* �������ܣ���������������
* ���������
* ���������
* �� �� ֵ��
* �������ڣ�2018��01��01��
* ע    �⣺ 
* �޸ļ�¼��
*********************************************************************************************************/
static void AckHeartBeat(void)
{
	// BLE�豸: {"state":"connected"}
	UART_SendStringLen("{\"state\":\"connected\"}\r\n", sizeof("{\"state\":\"connected\"}\r\n") - 1);
}



/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�void BlinkerInitialize(void)
* 
* �������ܣ���ƿƼ�APP�ӿڳ�ʼ��
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* 
* �޸ļ�¼��
*********************************************************************************************************/
void BlinkerInitialize(void)
{
	// ���ڽ��ղ�����ʼ��
	g_strUartRecvBuffer.RecvStep  = 0;
	g_strUartRecvBuffer.RecvCount = 0;
	g_strUartRecvBuffer.MarkCount = 0;
	
	// ���ڽ���IRQ����
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);

	// LED����״̬����
	g_strBlinkerData.led_switch = 0;
	g_strBlinkerData.led_mode   = 0;
	g_strBlinkerData.iSwwChange = 0;
	g_strBlinkerData.HeartTick  = -1ul;

	// ��ȡEEPROM����ֵ
 	EEPROM_ReadByte(EEPROM_ADR_WWBRIGHT, (uint8_t *)(&g_strBlinkerData.wwBright));
	//	PWM_Drive_PD4(g_strBlinkerData.wwBright * 3);
	if (g_strBlinkerData.wwBright > LED_BRIGH_MAX)
	{
		g_strBlinkerData.wwBright = LED_BRIGH_MAX / 3;
	}
	if(g_strBlinkerData.wwBright > 0)
	{
		g_strBlinkerData.led_switch = 1;
		
	}
	PWM_SetBright(g_strBlinkerData.wwBright);
}

/*********************************************************************************************************
* �������ƣ�void BlinkerProcess(void)
* 
* �������ܣ���ƿƼ�APP����������
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺1.���ID�̶�����
*			2.����ѭ����ʵʱ����	!!!!!!!!!!!
* 
* �޸ļ�¼��
*********************************************************************************************************/
void BlinkerProcess(void)
{
	char     *pBuf;
	uint32_t sliderValue;
	uint32_t r_value, g_value, b_value, bright_value;
	
	// ��⴮�ڽ��շ�?
	if (UART_CheckReceive())
	{
		// 1.������ͬ������: {"get":"state"}\n
		if (NULL != strstr((char *)g_strUartRecvBuffer.Buffer, "\"get\":\"state\""))
		{
			AckHeartBeat();
			g_strBlinkerData.HeartTick = g_u32_1msTick;

			// ͬ���豸״̬
			if (0 == g_strBlinkerData.led_switch)
			{
				AckSwitchOff();
			} 
			else
			{
				AckSwitchOn();

				if (0 == g_strBlinkerData.led_mode)
				{
					// ������ֵ
					AckBrightData();
				} 
				else
				{
					// �߲ʵƹ�
					AckColorData();
				}
			}
		}
	
		// 2.���ذ���: {"btn-abc":"on"}\n, {"btn-abc":"off"}\n
		if (NULL != strstr((char *)g_strUartRecvBuffer.Buffer, "\"btn-abc\":\"on\""))
		{
			g_strBlinkerData.led_switch = 1;
			AckSwitchOn();

			// ͬ���豸״̬
			if (0 == g_strBlinkerData.led_mode)
			{
				PWM_CloseColor();
				PWM_SetBright(g_strBlinkerData.wwBright);

				AckBrightData();
			} 
			else
			{
				PWM_CloseBright();
				PWM_SetColor(g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);

				AckColorData();
			}
		}

		// 3.���ذ���: {"btn-abc":"off"}
		if (NULL != strstr((char *)g_strUartRecvBuffer.Buffer, "\"btn-abc\":\"off\""))
		{
			g_strBlinkerData.led_switch = 0;
			AckSwitchOff();

			// ͬ���豸״̬
			PWM_CloseAll();
		}
		
		// 4.���������: {"ran-ww":30}\n
		pBuf = strstr((char *)g_strUartRecvBuffer.Buffer, "\"ran-ww\":");
		if (NULL != pBuf)
		{
			sliderValue = 0;

			if (1 == sscanf(pBuf, "\"ran-ww\":%u", &sliderValue))
			{
				// ͬ���豸״̬
				g_strBlinkerData.wwBright   = sliderValue;
				g_strBlinkerData.iSwwChange = 1;
				g_strBlinkerData.wwBrightTick = g_u32_1msTick;

				g_strBlinkerData.led_mode   = 0;
				g_strBlinkerData.led_switch = 1;
				AckSwitchOn();													//��������״̬Ϊ��
		
				PWM_CloseColor();												//�ر����вʵ�
				PWM_SetBright(g_strBlinkerData.wwBright);  				        //PWMռ�ձȴ���

				AckBrightData();												//���⻬���ٶ�̫��������
			}
		}

		// 5.��ɫ���: {"col-led":[R,G,B,Brightness]}\n
		pBuf = strstr((char *)g_strUartRecvBuffer.Buffer, "\"col-led\":[");
		if (NULL != pBuf)
		{
			r_value = 0;
			g_value = 0;
			b_value = 0;
			bright_value = 0;

			if (4 == sscanf(pBuf, "\"col-led\":[%u,%u,%u,%u]", &r_value, &g_value, &b_value, &bright_value))
			{
				// ͬ���豸״̬
				g_strBlinkerData.r_value = r_value;
				g_strBlinkerData.g_value = g_value;
				g_strBlinkerData.b_value = b_value;
				g_strBlinkerData.bright_value = bright_value;

				g_strBlinkerData.led_mode   = 1;
				g_strBlinkerData.led_switch = 1;
				AckSwitchOn();
			
				PWM_CloseBright();
				PWM_SetColor(g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);
					
				AckColorData();
			}
		}	
		
		// ������ɣ���������
		UART_InitReceive();
	}

	// �������ȱ���
	if (1 == g_strBlinkerData.iSwwChange)
	{
		if ((uint32_t)(g_u32_1msTick - g_strBlinkerData.wwBrightTick) >=  LED_BRIGHT_SAVE_TIME)
		{
			g_strBlinkerData.iSwwChange = 0;

 			EEPROM_WriteByte(EEPROM_ADR_WWBRIGHT, g_strBlinkerData.wwBright);
		}
	}
}

/*********************************************************************************************************
* �������ƣ�void BlinkAdjustBright(void)
* 
* �������ܣ����ݵƹ�״̬���Զ�ѭ�������ƹ�����
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺1.������Χ0 ~ 255������25
* 
* �޸ļ�¼��
*********************************************************************************************************/
void BlinkAdjustBright(void)
{
	// ��������
	if (1 == g_strBlinkerData.led_switch)
	{
		if (0 == g_strBlinkerData.led_mode)
		{
			if (LED_BRIGH_MAX == g_strBlinkerData.wwBright)
			{
				g_strBlinkerData.wwBright = 0;
			} 
			else
			{
				g_strBlinkerData.wwBright += LED_ADJUST_STEP;

				if (g_strBlinkerData.wwBright > LED_BRIGH_MAX)
				{
					g_strBlinkerData.wwBright = LED_BRIGH_MAX;
				}
			}

			PWM_SetBright(g_strBlinkerData.wwBright);
			AckBrightData();
		} 
		else if(1 == g_strBlinkerData.led_mode)
		{
			g_strBlinkerData.bright_value = 255;
			if(Ledchose == 1)
			{
				if (LED_BRIGH_MAX == g_strBlinkerData.r_value)
				{
					g_strBlinkerData.r_value = 0;
				} 
				else
				{
					g_strBlinkerData.r_value += LED_ADJUST_STEP;
					
					if (g_strBlinkerData.r_value > LED_BRIGH_MAX)
					{
						g_strBlinkerData.r_value = LED_BRIGH_MAX;
					}
					
				}
				PWM_SetColor(g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);
			}
			
			if(Ledchose == 2)
			{
				if (LED_BRIGH_MAX == g_strBlinkerData.g_value)
				{
					g_strBlinkerData.g_value = 0;
				} 
				else
				{
					g_strBlinkerData.g_value += LED_ADJUST_STEP;
					
					if (g_strBlinkerData.g_value > LED_BRIGH_MAX)
					{
						g_strBlinkerData.g_value = LED_BRIGH_MAX;
					}
				}
				PWM_SetColor(g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);
			}
			
			if(Ledchose == 3)
			{
				if (LED_BRIGH_MAX == g_strBlinkerData.b_value)
				{
					g_strBlinkerData.b_value = 0;
				} 
				else
				{
					g_strBlinkerData.b_value += LED_ADJUST_STEP;
					
					if (g_strBlinkerData.b_value > LED_BRIGH_MAX)
					{
						g_strBlinkerData.b_value = LED_BRIGH_MAX;
					}
				}
				PWM_SetColor(g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);
			}
			AckColorData();
		}	
	}
	if(Ledchose == 4)
	{
		MusicVolumeMore();
	}
}


void BlinkAdjustBright2(void)
{
	// ��������
	if (1 == g_strBlinkerData.led_switch)
	{
		if (0 == g_strBlinkerData.led_mode)
		{
			if (LED_BRIGH_MAX == g_strBlinkerData.wwBright)
			{
				g_strBlinkerData.wwBright = 0;
			} 
			else
			{
				

				if (g_strBlinkerData.wwBright >= LED_ADJUST_STEP)
				{
					g_strBlinkerData.wwBright -= LED_ADJUST_STEP;
					
				}
				else
				{
					g_strBlinkerData.wwBright = 0;
				}
			}

			PWM_SetBright(g_strBlinkerData.wwBright);
			AckBrightData();
		} 
		else if(1 == g_strBlinkerData.led_mode)
		{
			g_strBlinkerData.bright_value = 255;
			
			if(Ledchose == 1)
			{
				if (LED_BRIGH_MAX == g_strBlinkerData.r_value)
				{
					g_strBlinkerData.r_value = 0;
				} 
				else
				{
					
					
					if (g_strBlinkerData.r_value >= LED_ADJUST_STEP)
					{
						g_strBlinkerData.r_value -= LED_ADJUST_STEP;
						
					}
					else
					{
						g_strBlinkerData.r_value = 0;
					}
					
				}
				PWM_SetColor(g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);
			}
			
			if(Ledchose == 2)
			{
				if (LED_BRIGH_MAX == g_strBlinkerData.g_value)
				{
					g_strBlinkerData.r_value = 0;
				} 
				else
				{
					if (g_strBlinkerData.g_value >= LED_ADJUST_STEP)
					{
						g_strBlinkerData.g_value -= LED_ADJUST_STEP;
						
					}
					else
					{
						g_strBlinkerData.g_value = 0;
					}
				PWM_SetColor(g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);
				}
			}
			if(Ledchose == 3)
			{
				
				if (LED_BRIGH_MAX == g_strBlinkerData.b_value)
				{
					g_strBlinkerData.r_value = 0;
				} 
				else
				{
					if (g_strBlinkerData.b_value >= LED_ADJUST_STEP)
					{
						g_strBlinkerData.b_value -= LED_ADJUST_STEP;
						
					}
					else
					{
						g_strBlinkerData.b_value = 0;
					}
				PWM_SetColor(g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);
				}
			}
			AckColorData();
		}	
	}
	if(Ledchose == 4)
	{
		MusicVolumeLess();
	}
}
/*********************************************************************************************************
* �������ƣ�void BlinkToggleLed(void)
* 
* �������ܣ�����LED��
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
void BlinkToggleLed(void)
{
//	static uint32_t ledState;
	if (0 == g_strBlinkerData.led_switch)
	{
		// ͬ���豸״̬
		if (0 == g_strBlinkerData.led_mode)
		{
			PWM_CloseColor();
			PWM_SetBright(g_strBlinkerData.wwBright);
			
			AckBrightData();
		} 
		else
		{
			PWM_CloseBright();
			PWM_SetColor(g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);
			
			AckColorData();
		}

		g_strBlinkerData.led_switch = 1;
		AckSwitchOn();
	} 
	else
	{
		// ͬ���豸״̬
		PWM_CloseAll();
		g_strBlinkerData.led_switch = 0;
		g_strBlinkerData.led_mode = 0;
		Ledchose = 0;
	
		AckSwitchOff();
	}
}
/*********************************************************************************************************
* �������ƣ�void BlinkToggleLed(void)
* 
* �������ܣ�����LED��
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
void BlinkColorchose(void)
{
	
	if (1 == g_strBlinkerData.led_switch)
	{
		if( g_strBlinkerData.led_mode == 0)
		{
			g_strBlinkerData.led_mode = 1;
		}

			switch(g_strBlinkerData.led_mode)
			{
				case 1:
				Ledchose ++;
				if(Ledchose > 4)
				{
					g_strBlinkerData.led_mode = 0;
					Ledchose = 0;
				}
				break;
			}
//			PWM_CloseAll();
				
	}
	

}
