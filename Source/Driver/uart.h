/*********************************************************************************************************
* 模块名称：uart.h
* 
* 摘    要：USART1接口初始化及数据收发（for 点灯科技蓝牙应用，JDY-18模块）
* 
* 当前版本：1.0.0
* 
* 作    者：ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* 完成日期：2021年05月11日 
* 
* 内    容：1、接口初始化
*			2、数据收发
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
#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C" {
#endif
	
	
/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define USART1_CLK				RCC_APB2Periph_USART1			// USART1模块时间

#define USART1_TX_PORT			GPIOA							// TX引脚端口
#define USART1_TX_PIN			GPIO_Pin_3
#define USART1_TX_CLK			RCC_AHBPeriph_GPIOA
	
#define USART1_RX_PORT			GPIOB							// RX引脚端口
#define USART1_RX_PIN			GPIO_Pin_4
#define USART1_RX_CLK			RCC_AHBPeriph_GPIOB

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
typedef struct
{
	uint32_t RecvStep;		// 数据接收进程

	uint32_t StartTick;		// 开始接收时间

	uint32_t MarkCount;		// 标准字符接收数

	uint32_t RecvCount;		// 数据缓冲长度

	uint8_t Buffer[256];	// 数据缓冲
} USART_BufferDef;

extern USART_BufferDef	g_strUartRecvBuffer;			// 点灯APP接口数据缓冲器
extern uint8_t ch;
/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void UART_Initialize(void);							// USART1接口初始化

void UART_SendByte(uint8_t ch);							// USART1发送字符
void UART_SendStringLen(char *p, uint32_t len);			// USART1发送字符串

uint32_t UART_HaveRecvData(void);						// USART1有接收数据否？
uint32_t UART_GetRecvByte(void);						// 获得USART1的接收数据

void UART_InitReceive(void);							// 重置串口数据接收流程
uint32_t UART_CheckReceive(void);						// 检测串口数据接收完成否
	

#ifdef  __cplusplus
}
#endif

#endif  /* __USART_H */
