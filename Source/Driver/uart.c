/*********************************************************************************************************
* ģ�����ƣ�uart.c
* 
* ժ    Ҫ��USART1�ӿڳ�ʼ���������շ���for ��ƿƼ�����Ӧ�ã�JDY-18ģ�飩
* 
* ��ǰ�汾��1.0.0
* 
* ��    �ߣ�ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* ������ڣ�2021��05��11�� 
* 
* ��    �ݣ�1���ӿڳ�ʼ��
*			2�������շ�: ͬ��ͨ����ѯTXE��־�������ݣ����жϽ��յ����ݺ���뻺��
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

#include "uart.h"
#include "delay.h"
#include "03T.h"
/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define USART_RECV_COMPLETE		3						// �������ݰ�����������־ (ע����� !!!!!!!!!!!)
#define USART_RECV_SPACE_TIME	5						// ���ڽ��նϰ����ʱ��ms ��9600 = 1ms��
		
/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ȫ�ֱ���
*********************************************************************************************************/
uint8_t ch;
USART_BufferDef	g_strUartRecvBuffer;					// ��������ȫ�ֽ��ջ���

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
* �������ƣ�void USART_Initialization(void)
* 
* �������ܣ�USART1�ӿڳ�ʼ��: 9600, 8, N, 1
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺ƥ���ƿƼ�ʱ�����뿪���ж�
* 
* �޸ļ�¼��
*********************************************************************************************************/
void UART_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// Clock
	RCC_APB2PeriphClockCmd(USART1_CLK, ENABLE);
	RCC_AHBPeriphClockCmd(USART1_TX_CLK, ENABLE);
	RCC_AHBPeriphClockCmd(USART1_RX_CLK, ENABLE);

	// USART
	GPIO_InitStructure.GPIO_Pin  = USART1_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Disable;
	GPIO_Init(USART1_TX_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
	GPIO_Init(USART1_RX_PORT, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(USART1_TX_PORT, GPIO_PinSource3, GPIO_AF_1);
	GPIO_PinAFConfig(USART1_RX_PORT, GPIO_PinSource4, GPIO_AF_1);
	
	// 9600, 8, N, 1
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	// Enable 232
	USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_RXNE);
}

/*********************************************************************************************************
* �������ƣ�void USART_SendByte(uint8_t ch)
* 
* �������ܣ�USART1��������һ���ַ�
* 
* ���������ch = �����ַ�
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void UART_SendByte(uint8_t ch)
{
	// ��ѯTXE��־
	while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));
// 	while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TC));

	// ����
	USART_SendData(USART1, ch);
}

/*********************************************************************************************************
* �������ƣ�void USART_SendStringLen(uint8_t *pBuf, uint16_t len)
* 
* �������ܣ�USART1��������ָ�����ȵ��ַ�������
* 
* ���������pBuf = �����ַ������ݻ�������len = ���ͳ���
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
void UART_SendStringLen(char *pBuf, uint32_t len)
{
	while (0 != len)
	{
		UART_SendByte(*pBuf);

		pBuf++;
		len--;
	}
}

/*********************************************************************************************************
* �������ƣ�uint32_t USART_HaveRecvData(void)
* 
* �������ܣ����USART1�Ƿ����н������ݿ���ȡ
* 
* ���������None
* ���������None
* 
* �� �� ֵ��1 - �����ݣ�0 - ������
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺
* �޸ļ�¼��
*********************************************************************************************************/
uint32_t UART_HaveRecvData(void)
{
	if (SET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
	{
		return 1;
	} 
	else
	{
		return 0;
	}
}

/*********************************************************************************************************
* �������ƣ�uint32_t USART_GetRecvByte(void)
* 
* �������ܣ����USART1�Ľ������ݣ��ֽڣ�
* 
* ���������None
* ���������None
* 
* �� �� ֵ��-1 - �����ݣ�0x00 ~ 0xFF �����ֽ�
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺�ȼ�������ݺ󣬲ŵ��ñ�������ȡ����
* 
* �޸ļ�¼��
*********************************************************************************************************/
uint32_t UART_GetRecvByte(void)
{
	if (SET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
	{
		return USART_ReceiveData(USART1) & 0xFF;
	}
	else
	{
		return (uint32_t)(-1);
	}
}

/*********************************************************************************************************
* �������ƣ�void USART_InitReceive(void)
* 
* �������ܣ����ô������ݽ�������
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺���ڽ�������ʹ��ȫ�ֱ������ƣ�g_strUartRecvBuffer
* 
* �޸ļ�¼��
*********************************************************************************************************/
void UART_InitReceive(void)
{
	// �ر�ISR�жϣ���ֹ���������ͻ
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

	// ��������
	g_strUartRecvBuffer.RecvStep  = 0;
	g_strUartRecvBuffer.RecvCount = 0;
	g_strUartRecvBuffer.MarkCount = 0;

	// ����ISR
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

/*********************************************************************************************************
* �������ƣ�uint32_t USART_CheckReceive(void)
* 
* �������ܣ���⴮�����ݽ�����ɷ�
* 
* ���������None
* ���������None
* �� �� ֵ��0 = δ��ɣ�1 = Ҳ�����ݰ�
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺1.ʹ��ȫ�ֱ������壺g_strUartRecvBuffer
*			2.��JSON��ʽʶ��������ݳ���
*			3.���ֽڳ�ʱ�����ݰ�
*			4.����ѭ����ʵʱ���� !!!!!!!!!!!
* 
* �޸ļ�¼��
*********************************************************************************************************/
uint32_t UART_CheckReceive(void)
{
	// ��������ɷ�
	if (USART_RECV_COMPLETE == g_strUartRecvBuffer.RecvStep)
	{
		// JSONЭ�鴮��{" ":" "}
		if (g_strUartRecvBuffer.RecvCount >= 9)
		{
			return 1;
		} 
		else
		{
			// ���մ�����������
			UART_InitReceive();
		}
	}
	else
	{
		// �������з�
		if (0 != g_strUartRecvBuffer.RecvStep)
		{
			// �����ն���
			if ((uint32_t)(g_u32_1msTick - g_strUartRecvBuffer.StartTick) >=  USART_RECV_SPACE_TIME)
			{		
				// ���ճ�ʱ����������
				UART_InitReceive();
			}
		}
	}
	
	return 0;
}

/*********************************************************************************************************
* �������ƣ�void USART1_IRQHandler(void)
* 
* �������ܣ�ͨ��RXNE�жϽ������ݣ���������ƿƼ����ӿ�Э�����
* 
* ���������None
* ���������None
* �� �� ֵ��None
* 
* �������ڣ�2021��05��10��
* 
* ע    �⣺1.��󻺳���
* �޸ļ�¼��
*********************************************************************************************************/
void USART1_IRQHandler(void)
{

	// ��ѯ���ձ�־
	if (SET == USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		EstimateDate();
		// ��������
		ch = USART_ReceiveData(USART1);

		// ���ֽ���
		switch (g_strUartRecvBuffer.RecvStep)
		{
		case 0:
			// ������ַ�
			if ('{' == ch)
			{
				// �������ݵȴ�
				g_strUartRecvBuffer.RecvStep++;

				// ��־���ţ���ʼ
				g_strUartRecvBuffer.MarkCount = 1;

				// �洢����
				g_strUartRecvBuffer.Buffer[0] = ch;
				g_strUartRecvBuffer.RecvCount = 1;
				g_strUartRecvBuffer.StartTick = g_u32_1msTick;
			}
			break;

		case 1:
			// �洢����
			if (g_strUartRecvBuffer.RecvCount < (sizeof(g_strUartRecvBuffer.Buffer) - 2))
			{
				g_strUartRecvBuffer.Buffer[g_strUartRecvBuffer.RecvCount] = ch;
				g_strUartRecvBuffer.RecvCount++;
				g_strUartRecvBuffer.StartTick = g_u32_1msTick;
			}

			// ����ʶ��
			if ('{' == ch)
			{
				g_strUartRecvBuffer.MarkCount++;
			}
			else if ('}' == ch)
			{
				g_strUartRecvBuffer.MarkCount--;

				if (0 == g_strUartRecvBuffer.MarkCount)
				{
					// ĩ����
					g_strUartRecvBuffer.Buffer[g_strUartRecvBuffer.RecvCount] = 0;

					// ������Ϣ���
					g_strUartRecvBuffer.RecvStep++;
				}
			}
			break;

		case 2:
			// ��������
			if (0x0A == ch)
			{
				// ���ݰ��������
				g_strUartRecvBuffer.RecvStep = USART_RECV_COMPLETE;
			} 
			else
			{
				// �������ݰ������ý���
				g_strUartRecvBuffer.RecvStep  = 0;
				g_strUartRecvBuffer.RecvCount = 0;
				g_strUartRecvBuffer.MarkCount = 0;
			}
			break;
		}
	}
	
//		if (USART_GetITStatus(USART1,USART_IT_RTO) != RESET)
//		{
//			//��ʱ����
//			RecvTimeout();
//			//�����ʱ��־		
//			USART_ClearITPendingBit(USART1,USART_IT_RTO);				
//		}
//		
//		USART_ClearITPendingBit(USART1,USART_IT_ORE);		//��������־
	
}
