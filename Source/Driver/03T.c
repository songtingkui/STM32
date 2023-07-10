/*********************************************************************************************************
* ģ�����ƣ�03T.c
* 
* ժ    Ҫ��uart
* 
* ��ǰ�汾��1.0.0
* 
* ��    �ߣ�ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* ������ڣ�2021��05��11�� 
* 
* ��    �ݣ�1��03T��������
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
#include "03T.h"
#include "uart.h"
#include "blinker.h"
#include "delay.h"
/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define CHAGE_BRIGHT 		5											//03T���ȱ仯
#define CHAGE_MAX    	  (LED_BRIGH_MAX - CHAGE_BRIGHT)				//03T�����ж�ֵ
#define CHAGE_MIN          CHAGE_BRIGHT									//03T��С�ж�ֵ
/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                             ȫ�ֱ���
*********************************************************************************************************/
uint32_t g_u32ComRecvCompelet = 0;											//������Ч��־
uint32_t g_u32ComRecvCount = 0;												//���ռ���
uint8_t  g_u8ComRecvBuffer[MAX_COM_RECV_BUFFER + 1];						//���ջ�����
uint32_t g_u32ComRecv_mark;                  								//���յڶ��ַ���־
uint32_t g_32stateTrigger;													//LED�仯����
uint32_t g_32Sleeping_mark;
uint32_t Count_volume;

extern uint32_t g_u8ComRecvSoft;
/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
uint8_t nextMusic[4]       = {0xAA, 0x06, 0x00, 0xB0};		//��һ��
uint8_t upMusic[4]         = {0xAA, 0x05, 0x00, 0xAF};		//��һ��
uint8_t MusicVolumeUp[4]   = {0xAA, 0x14, 0x00, 0xBE};		//������
uint8_t MusicVolumeDown[4] = {0xAA, 0x15, 0x00, 0xBF};		//������
uint8_t MusicVolume_20[5]  = {0xAA, 0x13, 0x01, 0x14, 0xD2};		//����20
 
uint8_t MusicPlay[4] 	   = {0xAA, 0x02, 0x00, 0xAC};		//����
uint8_t MusicStop[4] 	   = {0xAA, 0x03, 0x00, 0xAD};		//��ͣ
uint8_t MusicClose[4] 	   = {0xAA, 0x04, 0x00, 0xAE};		//ֹͣ
uint8_t Music[15] 	  	   = {0xAA, 0x08, 0x0B, 0x00, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x31, 0x2A, 0x3F, 0x3F, 0x3F, 0xC4};		//00001����

uint8_t Music_1[6] 	  	   = {0xAA, 0x07, 0x02, 0x00, 0x01, 0xB4};		//00001����


/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
* �������ƣ�void EstimateDate(void)
* 
* �������ܣ��ж��Ƿ�Ϊ��Ч����
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��12��04��
* 
* ע    �⣺
* �޸ļ�¼��

*********************************************************************************************************/
void EstimateDate(void)
{
	ch = USART_ReceiveData(USART1);
	switch(g_u32ComRecvCount)
	{
		case 0:
			if (ch == 0xAA)
			{
				g_u8ComRecvBuffer[g_u32ComRecvCount] = ch;
				g_u32ComRecvCount++;
			}
			
		break;
			
		case 1:
			if (ch == 0x44)
			{
				g_u8ComRecvBuffer[g_u32ComRecvCount] = ch;
				g_u32ComRecvCount++;
				g_u32ComRecv_mark = 1;
			}
			else
			{
				g_u32ComRecvCount = 0;
			}
			
		break;
			
		case 2:
			if (/*(comDate >= 0) &&   �޷������κ����0*/(ch <= 100))
			{
				g_u8ComRecvBuffer[g_u32ComRecvCount] = ch;
				g_u32ComRecvCount++;
				
				g_u32ComRecvCompelet = 1;
			}
			else
			{
				g_u32ComRecvCount = 0;
			}
			
		break;				
			
	}
}

/*********************************************************************************************************
* �������ƣ�void EstimateDate(void)
* 
* �������ܣ��ж��Ƿ�ʱ
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��12��04��
* 
* ע    �⣺
* �޸ļ�¼��

*********************************************************************************************************/
void RecvTimeout(void)
{
	if (g_u32ComRecvCount < 3)
	{
		g_u32ComRecvCount = 0;
	}
}

/*********************************************************************************************************
* �������ƣ�void USART_AnalysisDate();
* 
* �������ܣ�USART���ڽ������ݷ���
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��12��04��
* 
* ע    �⣺
* �޸ļ�¼��

*********************************************************************************************************/
void USART_AnalysisDate(void)
{
//	if (0 != g_u32ComRecvCompelet)
//	{
		if (g_u32ComRecv_mark == 1)	
		{
			switch(g_u8ComRecvSoft)
			{
				case 0x00:
				g_strBlinkerData.wwBright = 100;
				g_32stateTrigger = 1;
				break;
				
				case 0x01:										
				g_strBlinkerData.wwBright = 0;
				g_32stateTrigger = 1;
				
				break;
				
				case 0x02:										//��ȫ���ƹ�
				g_strBlinkerData.wwBright  = 100;
				g_strBlinkerData.r_value   = 100;
				g_strBlinkerData.g_value   = 100;
				g_strBlinkerData.b_value   = 100;
				g_32stateTrigger = 1;
				break;				
				
				case 0x03:										//�ر�ȫ���ƹ�|�ػ�
				g_strBlinkerData.wwBright  = 0;
				g_strBlinkerData.r_value   = 0;
				g_strBlinkerData.g_value   = 0;
				g_strBlinkerData.b_value   = 0;
				Ledchose = 0;
				g_32stateTrigger = 2;
				break;
				
				case 0x04:										//�򿪺��|�л����
				g_strBlinkerData.r_value = 100;
				g_32stateTrigger = 1;
				Ledchose = 1;
				g_strBlinkerData.led_mode = 1;
				break;
				
				case 0x05:										//�رպ��
				g_strBlinkerData.r_value = 0;
				g_32stateTrigger = 1;
				Ledchose = 1;
				g_strBlinkerData.led_mode = 1;
				break;
				
				case 0x06:										//������|�л�����
				g_strBlinkerData.b_value = 100;
				g_32stateTrigger = 1;
				Ledchose = 3;
				g_strBlinkerData.led_mode = 1;
				break;
				
				case 0x07:										//�ر�����
				g_strBlinkerData.b_value = 0;
				g_32stateTrigger = 1;
				Ledchose = 3;
				g_strBlinkerData.led_mode = 1;
				break;
					
				case 0x08:										//���̵�|�л��̵�
				g_strBlinkerData.g_value = 100;
				g_32stateTrigger = 1;
				Ledchose = 2;
				g_strBlinkerData.led_mode = 1;
				break;
				
				case 0x09:										//�ر��̵�
				g_strBlinkerData.g_value = 0;
				g_32stateTrigger = 1;
				Ledchose = 2;
				g_strBlinkerData.led_mode = 1;
				break;
				case 0x0A: 										//�������ȣ�ů�׵ƣ�					
				g_strBlinkerData.led_mode =0;
				BlinkAdjustBright();
//				if (g_strBlinkerData.wwBright <= CHAGE_MAX)
//				{
//					g_strBlinkerData.wwBright += CHAGE_BRIGHT;
//				}
//				g_32stateTrigger = 1;
				break;
				
				case 0x0B:										//��������
					
				if (g_strBlinkerData.wwBright >= CHAGE_MIN)
				{
					g_strBlinkerData.wwBright -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x0C: 										//���ߺ��					
				if (g_strBlinkerData.r_value <= CHAGE_MAX)
				{
					g_strBlinkerData.r_value += CHAGE_BRIGHT;
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x0D:										//�������	
				if (g_strBlinkerData.r_value >= CHAGE_MIN)
				{
					g_strBlinkerData.r_value -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x0E: 										//��������					
				if (g_strBlinkerData.b_value <= CHAGE_MAX)
				{
					g_strBlinkerData.b_value += CHAGE_BRIGHT;
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x0F:										//��������	
				if (g_strBlinkerData.b_value >= CHAGE_MIN)
				{
					g_strBlinkerData.b_value -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x10: 										//�����̵�					
				if (g_strBlinkerData.g_value <= CHAGE_MAX)
				{
					g_strBlinkerData.g_value += CHAGE_BRIGHT;
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x11:										//�����̵�	
				if (g_strBlinkerData.g_value >= CHAGE_MIN)
				{
					g_strBlinkerData.g_value -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;
				break;	
				
				case 0x12:										//�����ʵ���������
				if (g_strBlinkerData.bright_value <= CHAGE_MAX)
				{
					g_strBlinkerData.bright_value += CHAGE_BRIGHT;
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x13:										//�����ʵ���������
				if (g_strBlinkerData.bright_value >= CHAGE_MIN)
				{
					g_strBlinkerData.bright_value -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;				
				break;
				
				case 0x14:										//˯��ģʽ
				g_32Sleeping_mark = 1;			
				UART_SendStringLen((char *) Music_1,6);
				Count_volume = 30;
				break;
				
				case 0x15:										//����
				UART_SendStringLen((char *) MusicPlay,4);
				UART_SendStringLen((char *) MusicVolume_20,5);
				break;
				
				case 0x16:										//��ͣ
				UART_SendStringLen((char *) MusicStop,4);
				break;
								
				case 0x17:										//��һ��
				UART_SendStringLen((char *) upMusic,4);
				break;
				case 0x18:										//��һ��
				UART_SendStringLen((char *) nextMusic,4);
				break;
				case 0x19:										//������һ��|��������|��������
				MusicVolumeMore();
				break;
				
				case 0x1A:										//����Сһ��|��С����|��������
				MusicVolumeLess();
				break;
			}
			LED_Choose();
		}
			g_u32ComRecvCount =0;
			g_u32ComRecvCompelet =0;
			g_u32ComRecv_mark =0;
//	}
}

/*********************************************************************************************************
* �������ƣ�void LED_Choose(void)
* 
* �������ܣ�LED�Ƶ�״̬
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��12��04��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void LED_Choose(void)
{
	if (g_32stateTrigger == 1)
	{	
		
		g_32Sleeping_mark = 0;
		//��������״̬Ϊ��	
		UART_SendStringLen("{\"btn-abc\":{\"swi\":\"on\",\"tex\":\"\xe5\xb7\xb2\xe5\xbc\x80\xe7\x81\xaf\"}}\r\n", sizeof("{\"btn-abc\":{\"swi\":\"on\",\"tex\":\"\xe5\xb7\xb2\xe5\xbc\x80\xe7\x81\xaf\"}}\r\n") - 1);											
		g_strBlinkerData.led_switch = 1;
//		PWM_Drive_PD1((g_strBlinkerData.b_value  * B_PWM_PULSE ) / LED_BRIGH_MAX);
//		PWM_Drive_PD2((g_strBlinkerData.r_value  * R_PWM_PULSE ) / LED_BRIGH_MAX);
//		PWM_Drive_PD3((g_strBlinkerData.g_value  * G_PWM_PULSE ) / LED_BRIGH_MAX);
		PWM_Drive_PD4((g_strBlinkerData.wwBright * WW_PWM_PULSE) / LED_BRIGH_MAX);
		if(g_strBlinkerData.bright_value == 0)
		{
			g_strBlinkerData.bright_value = 100;
		}
		PWM_Drive_PD1((g_strBlinkerData.b_value * B_PWM_PULSE * g_strBlinkerData.bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
		PWM_Drive_PD2((g_strBlinkerData.r_value * R_PWM_PULSE * g_strBlinkerData.bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
		PWM_Drive_PD3((g_strBlinkerData.g_value * G_PWM_PULSE * g_strBlinkerData.bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
	}
	if(g_32stateTrigger == 2)
	{
	g_32Sleeping_mark = 0;
	// ����״̬: {"btn-abc":{"swi":"off","tex":"�ѹص�"}}  
	UART_SendStringLen("{\"btn-abc\":{\"swi\":\"off\",\"tex\":\"\xe5\xb7\xb2\xe5\x85\xb3\xe7\x81\xaf\"}}\r\n", sizeof("{\"btn-abc\":{\"swi\":\"off\",\"tex\":\"\xe5\xb7\xb2\xe5\x85\xb3\xe7\x81\xaf\"}}\r\n") - 1);
	PWM_Drive_PD1(0);
	PWM_Drive_PD2(0);
	PWM_Drive_PD3(0);
	PWM_Drive_PD4(0); 
	g_strBlinkerData.led_switch = 0;
	}
	
	
	g_32stateTrigger = 0;
	
}

/*********************************************************************************************************
* �������ƣ�void Sleeping(void)
* 
* �������ܣ�LED�Ƶ�״̬
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��12��04��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void Sleeping(void)
{

	if(1 == g_32Sleeping_mark)
	{
		if(g_u32_1msTick %1000 == 0)
		{
			
			if(Count_volume > 1)
			{
			UART_SendStringLen((char *) MusicVolumeDown,4);
			Count_volume --;
			}
				
			
			if(g_strBlinkerData.bright_value >= 5)
			{
				g_strBlinkerData.bright_value = g_strBlinkerData.bright_value - 5;
			}
			else
			{
				g_strBlinkerData.bright_value = 0;
			}
			
			if(g_strBlinkerData.wwBright >= 5)
			{
				g_strBlinkerData.wwBright = g_strBlinkerData.wwBright - 5;
			}
			else
			{
				g_strBlinkerData.wwBright = 0;
			}
		
			if(g_strBlinkerData.bright_value == 0 && g_strBlinkerData.wwBright == 0 && Count_volume == 1)
			{
				 g_32Sleeping_mark = 0;
				 UART_SendStringLen((char *) MusicStop,4);
			}
		}
		PWM_Drive_PD1((g_strBlinkerData.b_value * B_PWM_PULSE * g_strBlinkerData.bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
		PWM_Drive_PD2((g_strBlinkerData.r_value * R_PWM_PULSE * g_strBlinkerData.bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
		PWM_Drive_PD3((g_strBlinkerData.g_value * G_PWM_PULSE * g_strBlinkerData.bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
		PWM_Drive_PD4((g_strBlinkerData.wwBright * WW_PWM_PULSE) / LED_BRIGH_MAX);
		
	}
}



/*********************************************************************************************************
* �������ƣ�void USART1_IRQHandler()
* 
* �������ܣ�USART1�ж�
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��12��04��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void USART1_IRQHandler4()
{
	
	if (USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{
		 EstimateDate();														//�жϽ��������Ƿ���Ч
	}
	
	if (USART_GetITStatus(USART1,USART_IT_RTO) != RESET)
	{
		//��ʱ����
		RecvTimeout();
		//�����ʱ��־		
		USART_ClearITPendingBit(USART1,USART_IT_RTO);				
	}
	
	USART_ClearITPendingBit(USART1,USART_IT_ORE);		//��������־
	
}

/*********************************************************************************************************
* �������ƣ�void MusicVolumeMore(void)
* 
* �������ܣ�������
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��12��04��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void MusicVolumeMore(void)
{
	UART_SendStringLen((char *) MusicVolumeUp,4);
}
/*********************************************************************************************************
* �������ƣ�void MusicVolumeLess(void)
* 
* �������ܣ�������
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��12��04��
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void MusicVolumeLess(void)
{
	UART_SendStringLen((char *) MusicVolumeDown,4);
}	
