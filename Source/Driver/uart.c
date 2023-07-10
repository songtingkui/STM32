/*********************************************************************************************************
* 模块名称：uart.c
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
*			2、数据收发: 同步通过查询TXE标志发送数据，当中断接收到数据后存入缓冲
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

#include "uart.h"
#include "delay.h"
#include "03T.h"
/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define USART_RECV_COMPLETE		3						// 串口数据包接收完整标志 (注意调整 !!!!!!!!!!!)
#define USART_RECV_SPACE_TIME	5						// 串口接收断包间隔时间ms （9600 = 1ms）
		
/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              全局变量
*********************************************************************************************************/
uint8_t ch;
USART_BufferDef	g_strUartRecvBuffer;					// 串口数据全局接收缓冲

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
* 函数名称：void USART_Initialization(void)
* 
* 函数功能：USART1接口初始化: 9600, 8, N, 1
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：匹配点灯科技时，必须开启中断
* 
* 修改记录：
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
* 函数名称：void USART_SendByte(uint8_t ch)
* 
* 函数功能：USART1阻塞发送一个字符
* 
* 输入参数：ch = 发送字符
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：
* 修改记录：
*********************************************************************************************************/
void UART_SendByte(uint8_t ch)
{
	// 查询TXE标志
	while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));
// 	while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TC));

	// 发送
	USART_SendData(USART1, ch);
}

/*********************************************************************************************************
* 函数名称：void USART_SendStringLen(uint8_t *pBuf, uint16_t len)
* 
* 函数功能：USART1阻塞发送指定长度的字符串数据
* 
* 输入参数：pBuf = 发送字符串数据缓冲器；len = 发送长度
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：
* 修改记录：
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
* 函数名称：uint32_t USART_HaveRecvData(void)
* 
* 函数功能：检测USART1是否已有接收数据可提取
* 
* 输入参数：None
* 输出参数：None
* 
* 返 回 值：1 - 有数据，0 - 无数据
* 
* 创建日期：2021年05月10日
* 
* 注    意：
* 修改记录：
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
* 函数名称：uint32_t USART_GetRecvByte(void)
* 
* 函数功能：获得USART1的接收数据（字节）
* 
* 输入参数：None
* 输出参数：None
* 
* 返 回 值：-1 - 无数据，0x00 ~ 0xFF 接收字节
* 
* 创建日期：2021年05月10日
* 
* 注    意：先检测有数据后，才调用本函数提取数据
* 
* 修改记录：
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
* 函数名称：void USART_InitReceive(void)
* 
* 函数功能：重置串口数据接收流程
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：串口接收流程使用全局变量控制：g_strUartRecvBuffer
* 
* 修改记录：
*********************************************************************************************************/
void UART_InitReceive(void)
{
	// 关闭ISR中断，防止后续代码冲突
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

	// 重置流程
	g_strUartRecvBuffer.RecvStep  = 0;
	g_strUartRecvBuffer.RecvCount = 0;
	g_strUartRecvBuffer.MarkCount = 0;

	// 开启ISR
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

/*********************************************************************************************************
* 函数名称：uint32_t USART_CheckReceive(void)
* 
* 函数功能：检测串口数据接收完成否？
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：0 = 未完成，1 = 也有数据包
* 
* 创建日期：2021年05月10日
* 
* 注    意：1.使用全局变量缓冲：g_strUartRecvBuffer
*			2.按JSON格式识别接收数据长度
*			3.按字节超时断数据包
*			4.必须循环，实时调用 !!!!!!!!!!!
* 
* 修改记录：
*********************************************************************************************************/
uint32_t UART_CheckReceive(void)
{
	// 检测接收完成否？
	if (USART_RECV_COMPLETE == g_strUartRecvBuffer.RecvStep)
	{
		// JSON协议串：{" ":" "}
		if (g_strUartRecvBuffer.RecvCount >= 9)
		{
			return 1;
		} 
		else
		{
			// 接收错误，重启进程
			UART_InitReceive();
		}
	}
	else
	{
		// 检测接收中否？
		if (0 != g_strUartRecvBuffer.RecvStep)
		{
			// 检测接收断码
			if ((uint32_t)(g_u32_1msTick - g_strUartRecvBuffer.StartTick) >=  USART_RECV_SPACE_TIME)
			{		
				// 接收超时，重启进程
				UART_InitReceive();
			}
		}
	}
	
	return 0;
}

/*********************************************************************************************************
* 函数名称：void USART1_IRQHandler(void)
* 
* 函数功能：通过RXNE中断接收数据，并按“点灯科技”接口协议分组
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：1.最大缓冲器
* 修改记录：
*********************************************************************************************************/
void USART1_IRQHandler(void)
{

	// 查询接收标志
	if (SET == USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		EstimateDate();
		// 接收数据
		ch = USART_ReceiveData(USART1);

		// 区分进程
		switch (g_strUartRecvBuffer.RecvStep)
		{
		case 0:
			// 检测首字符
			if ('{' == ch)
			{
				// 进入数据等待
				g_strUartRecvBuffer.RecvStep++;

				// 标志符号：开始
				g_strUartRecvBuffer.MarkCount = 1;

				// 存储数据
				g_strUartRecvBuffer.Buffer[0] = ch;
				g_strUartRecvBuffer.RecvCount = 1;
				g_strUartRecvBuffer.StartTick = g_u32_1msTick;
			}
			break;

		case 1:
			// 存储数据
			if (g_strUartRecvBuffer.RecvCount < (sizeof(g_strUartRecvBuffer.Buffer) - 2))
			{
				g_strUartRecvBuffer.Buffer[g_strUartRecvBuffer.RecvCount] = ch;
				g_strUartRecvBuffer.RecvCount++;
				g_strUartRecvBuffer.StartTick = g_u32_1msTick;
			}

			// 检测标识符
			if ('{' == ch)
			{
				g_strUartRecvBuffer.MarkCount++;
			}
			else if ('}' == ch)
			{
				g_strUartRecvBuffer.MarkCount--;

				if (0 == g_strUartRecvBuffer.MarkCount)
				{
					// 末填零
					g_strUartRecvBuffer.Buffer[g_strUartRecvBuffer.RecvCount] = 0;

					// 数据信息完成
					g_strUartRecvBuffer.RecvStep++;
				}
			}
			break;

		case 2:
			// 结束码检测
			if (0x0A == ch)
			{
				// 数据包接收完成
				g_strUartRecvBuffer.RecvStep = USART_RECV_COMPLETE;
			} 
			else
			{
				// 错误数据包，重置接收
				g_strUartRecvBuffer.RecvStep  = 0;
				g_strUartRecvBuffer.RecvCount = 0;
				g_strUartRecvBuffer.MarkCount = 0;
			}
			break;
		}
	}
	
//		if (USART_GetITStatus(USART1,USART_IT_RTO) != RESET)
//		{
//			//超时处理
//			RecvTimeout();
//			//清除超时标志		
//			USART_ClearITPendingBit(USART1,USART_IT_RTO);				
//		}
//		
//		USART_ClearITPendingBit(USART1,USART_IT_ORE);		//清除溢出标志
	
}
