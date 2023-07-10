/*********************************************************************************************************
* ģ�����ƣ�uart.h
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
*			2�������շ�
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
#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C" {
#endif
	
	
/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define USART1_CLK				RCC_APB2Periph_USART1			// USART1ģ��ʱ��

#define USART1_TX_PORT			GPIOA							// TX���Ŷ˿�
#define USART1_TX_PIN			GPIO_Pin_3
#define USART1_TX_CLK			RCC_AHBPeriph_GPIOA
	
#define USART1_RX_PORT			GPIOB							// RX���Ŷ˿�
#define USART1_RX_PIN			GPIO_Pin_4
#define USART1_RX_CLK			RCC_AHBPeriph_GPIOB

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
typedef struct
{
	uint32_t RecvStep;		// ���ݽ��ս���

	uint32_t StartTick;		// ��ʼ����ʱ��

	uint32_t MarkCount;		// ��׼�ַ�������

	uint32_t RecvCount;		// ���ݻ��峤��

	uint8_t Buffer[256];	// ���ݻ���
} USART_BufferDef;

extern USART_BufferDef	g_strUartRecvBuffer;			// ���APP�ӿ����ݻ�����
extern uint8_t ch;
/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void UART_Initialize(void);							// USART1�ӿڳ�ʼ��

void UART_SendByte(uint8_t ch);							// USART1�����ַ�
void UART_SendStringLen(char *p, uint32_t len);			// USART1�����ַ���

uint32_t UART_HaveRecvData(void);						// USART1�н������ݷ�
uint32_t UART_GetRecvByte(void);						// ���USART1�Ľ�������

void UART_InitReceive(void);							// ���ô������ݽ�������
uint32_t UART_CheckReceive(void);						// ��⴮�����ݽ�����ɷ�
	

#ifdef  __cplusplus
}
#endif

#endif  /* __USART_H */