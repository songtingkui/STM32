/*********************************************************************************************************
* 模块名称：03T.c
* 
* 摘    要：uart
* 
* 当前版本：1.0.0
* 
* 作    者：ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* 完成日期：2021年05月11日 
* 
* 内    容：1、03T语音处理
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
#include "03T.h"
#include "uart.h"
#include "blinker.h"
#include "delay.h"
/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define CHAGE_BRIGHT 		5											//03T亮度变化
#define CHAGE_MAX    	  (LED_BRIGH_MAX - CHAGE_BRIGHT)				//03T增大判断值
#define CHAGE_MIN          CHAGE_BRIGHT									//03T减小判断值
/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                             全局变量
*********************************************************************************************************/
uint32_t g_u32ComRecvCompelet = 0;											//接收有效标志
uint32_t g_u32ComRecvCount = 0;												//接收计数
uint8_t  g_u8ComRecvBuffer[MAX_COM_RECV_BUFFER + 1];						//接收缓冲区
uint32_t g_u32ComRecv_mark;                  								//接收第二字符标志
uint32_t g_32stateTrigger;													//LED变化触发
uint32_t g_32Sleeping_mark;
uint32_t Count_volume;

extern uint32_t g_u8ComRecvSoft;
/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
uint8_t nextMusic[4]       = {0xAA, 0x06, 0x00, 0xB0};		//下一首
uint8_t upMusic[4]         = {0xAA, 0x05, 0x00, 0xAF};		//上一首
uint8_t MusicVolumeUp[4]   = {0xAA, 0x14, 0x00, 0xBE};		//音量加
uint8_t MusicVolumeDown[4] = {0xAA, 0x15, 0x00, 0xBF};		//音量减
uint8_t MusicVolume_20[5]  = {0xAA, 0x13, 0x01, 0x14, 0xD2};		//音量20
 
uint8_t MusicPlay[4] 	   = {0xAA, 0x02, 0x00, 0xAC};		//播放
uint8_t MusicStop[4] 	   = {0xAA, 0x03, 0x00, 0xAD};		//暂停
uint8_t MusicClose[4] 	   = {0xAA, 0x04, 0x00, 0xAE};		//停止
uint8_t Music[15] 	  	   = {0xAA, 0x08, 0x0B, 0x00, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x31, 0x2A, 0x3F, 0x3F, 0x3F, 0xC4};		//00001播放

uint8_t Music_1[6] 	  	   = {0xAA, 0x07, 0x02, 0x00, 0x01, 0xB4};		//00001播放


/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/

/*********************************************************************************************************
* 函数名称：void EstimateDate(void)
* 
* 函数功能：判断是否为有效数据
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年12月04日
* 
* 注    意：
* 修改记录：

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
			if (/*(comDate >= 0) &&   无符号整形恒大于0*/(ch <= 100))
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
* 函数名称：void EstimateDate(void)
* 
* 函数功能：判断是否超时
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年12月04日
* 
* 注    意：
* 修改记录：

*********************************************************************************************************/
void RecvTimeout(void)
{
	if (g_u32ComRecvCount < 3)
	{
		g_u32ComRecvCount = 0;
	}
}

/*********************************************************************************************************
* 函数名称：void USART_AnalysisDate();
* 
* 函数功能：USART串口接收数据分析
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年12月04日
* 
* 注    意：
* 修改记录：

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
				
				case 0x02:										//打开全部灯光
				g_strBlinkerData.wwBright  = 100;
				g_strBlinkerData.r_value   = 100;
				g_strBlinkerData.g_value   = 100;
				g_strBlinkerData.b_value   = 100;
				g_32stateTrigger = 1;
				break;				
				
				case 0x03:										//关闭全部灯光|关机
				g_strBlinkerData.wwBright  = 0;
				g_strBlinkerData.r_value   = 0;
				g_strBlinkerData.g_value   = 0;
				g_strBlinkerData.b_value   = 0;
				Ledchose = 0;
				g_32stateTrigger = 2;
				break;
				
				case 0x04:										//打开红灯|切换红灯
				g_strBlinkerData.r_value = 100;
				g_32stateTrigger = 1;
				Ledchose = 1;
				g_strBlinkerData.led_mode = 1;
				break;
				
				case 0x05:										//关闭红灯
				g_strBlinkerData.r_value = 0;
				g_32stateTrigger = 1;
				Ledchose = 1;
				g_strBlinkerData.led_mode = 1;
				break;
				
				case 0x06:										//打开蓝灯|切换蓝灯
				g_strBlinkerData.b_value = 100;
				g_32stateTrigger = 1;
				Ledchose = 3;
				g_strBlinkerData.led_mode = 1;
				break;
				
				case 0x07:										//关闭蓝灯
				g_strBlinkerData.b_value = 0;
				g_32stateTrigger = 1;
				Ledchose = 3;
				g_strBlinkerData.led_mode = 1;
				break;
					
				case 0x08:										//打开绿灯|切换绿灯
				g_strBlinkerData.g_value = 100;
				g_32stateTrigger = 1;
				Ledchose = 2;
				g_strBlinkerData.led_mode = 1;
				break;
				
				case 0x09:										//关闭绿灯
				g_strBlinkerData.g_value = 0;
				g_32stateTrigger = 1;
				Ledchose = 2;
				g_strBlinkerData.led_mode = 1;
				break;
				case 0x0A: 										//调高亮度（暖白灯）					
				g_strBlinkerData.led_mode =0;
				BlinkAdjustBright();
//				if (g_strBlinkerData.wwBright <= CHAGE_MAX)
//				{
//					g_strBlinkerData.wwBright += CHAGE_BRIGHT;
//				}
//				g_32stateTrigger = 1;
				break;
				
				case 0x0B:										//调低亮度
					
				if (g_strBlinkerData.wwBright >= CHAGE_MIN)
				{
					g_strBlinkerData.wwBright -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x0C: 										//调高红灯					
				if (g_strBlinkerData.r_value <= CHAGE_MAX)
				{
					g_strBlinkerData.r_value += CHAGE_BRIGHT;
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x0D:										//调暗红灯	
				if (g_strBlinkerData.r_value >= CHAGE_MIN)
				{
					g_strBlinkerData.r_value -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x0E: 										//调高蓝灯					
				if (g_strBlinkerData.b_value <= CHAGE_MAX)
				{
					g_strBlinkerData.b_value += CHAGE_BRIGHT;
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x0F:										//调暗蓝灯	
				if (g_strBlinkerData.b_value >= CHAGE_MIN)
				{
					g_strBlinkerData.b_value -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x10: 										//调高绿灯					
				if (g_strBlinkerData.g_value <= CHAGE_MAX)
				{
					g_strBlinkerData.g_value += CHAGE_BRIGHT;
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x11:										//调暗绿灯	
				if (g_strBlinkerData.g_value >= CHAGE_MIN)
				{
					g_strBlinkerData.g_value -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;
				break;	
				
				case 0x12:										//调亮彩灯整体亮度
				if (g_strBlinkerData.bright_value <= CHAGE_MAX)
				{
					g_strBlinkerData.bright_value += CHAGE_BRIGHT;
				}
				g_32stateTrigger = 1;
				break;
				
				case 0x13:										//调暗彩灯整体亮度
				if (g_strBlinkerData.bright_value >= CHAGE_MIN)
				{
					g_strBlinkerData.bright_value -= CHAGE_BRIGHT;	
				}
				g_32stateTrigger = 1;				
				break;
				
				case 0x14:										//睡眠模式
				g_32Sleeping_mark = 1;			
				UART_SendStringLen((char *) Music_1,6);
				Count_volume = 30;
				break;
				
				case 0x15:										//播放
				UART_SendStringLen((char *) MusicPlay,4);
				UART_SendStringLen((char *) MusicVolume_20,5);
				break;
				
				case 0x16:										//暂停
				UART_SendStringLen((char *) MusicStop,4);
				break;
								
				case 0x17:										//上一首
				UART_SendStringLen((char *) upMusic,4);
				break;
				case 0x18:										//下一首
				UART_SendStringLen((char *) nextMusic,4);
				break;
				case 0x19:										//声音大一点|调大音量|调高音量
				MusicVolumeMore();
				break;
				
				case 0x1A:										//声音小一点|调小音量|调低音量
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
* 函数名称：void LED_Choose(void)
* 
* 函数功能：LED灯的状态
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年12月04日
* 注    意：
* 修改记录：
*********************************************************************************************************/
void LED_Choose(void)
{
	if (g_32stateTrigger == 1)
	{	
		
		g_32Sleeping_mark = 0;
		//反馈开关状态为开	
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
	// 开关状态: {"btn-abc":{"swi":"off","tex":"已关灯"}}  
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
* 函数名称：void Sleeping(void)
* 
* 函数功能：LED灯的状态
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年12月04日
* 注    意：
* 修改记录：
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
* 函数名称：void USART1_IRQHandler()
* 
* 函数功能：USART1中断
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年12月04日
* 注    意：
* 修改记录：
*********************************************************************************************************/
void USART1_IRQHandler4()
{
	
	if (USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{
		 EstimateDate();														//判断接收数据是否有效
	}
	
	if (USART_GetITStatus(USART1,USART_IT_RTO) != RESET)
	{
		//超时处理
		RecvTimeout();
		//清除超时标志		
		USART_ClearITPendingBit(USART1,USART_IT_RTO);				
	}
	
	USART_ClearITPendingBit(USART1,USART_IT_ORE);		//清除溢出标志
	
}

/*********************************************************************************************************
* 函数名称：void MusicVolumeMore(void)
* 
* 函数功能：音量加
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年12月04日
* 注    意：
* 修改记录：
*********************************************************************************************************/
void MusicVolumeMore(void)
{
	UART_SendStringLen((char *) MusicVolumeUp,4);
}
/*********************************************************************************************************
* 函数名称：void MusicVolumeLess(void)
* 
* 函数功能：音量减
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年12月04日
* 注    意：
* 修改记录：
*********************************************************************************************************/
void MusicVolumeLess(void)
{
	UART_SendStringLen((char *) MusicVolumeDown,4);
}	
