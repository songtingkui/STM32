/*********************************************************************************************************
* 模块名称：blinker.c
* 
* 摘    要：点灯科技接口协议分解
* 
* 当前版本：1.0.0
* 
* 作    者：ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* 完成日期：2021年05月11日 
* 
* 内    容：
*			
* 注    意：1、仅支持4种接口协议
*			2、使用全局缓冲，必须串口数据包接收完整后进行                                                               
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
#include <string.h>

#include "blinker.h"
#include "uart.h"
#include "pwm.h"
#include "03T.h"
#include "delay.h"
#include "eeprom.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define EEPROM_ADR_WWBRIGHT		0				// 照明亮度存储位置
#define LED_ADJUST_STEP			25				// LED按键调整亮度步进数
#define LED_BRIGHT_SAVE_TIME	4000			// LED亮度调整后，保存时间ms，最大4,194MS

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              全局变量
*********************************************************************************************************/
BLINKER_DataDef	g_strBlinkerData;				// LED控制状态数据
uint32_t Ledchose = 0;
/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
 
/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void PWM_CloseBright(void);				// 关闭照明
static void PWM_CloseColor(void);				// 关闭彩灯
static void PWM_CloseAll(void);					// PWM输出全部关闭

static void PWM_SetBright(uint32_t brightness);	// 设置照明输出
static void PWM_SetColor(uint32_t r_value, uint32_t g_value, uint32_t b_value, uint32_t bright_value);	// 设置七彩输出

static void AckBrightData(void);				// 反馈照明亮度数据
static void AckColorData(void);					// 反馈彩灯亮度数据
static void AckHeartBeat(void);					// 心跳包数据
static void AckSwitchOn(void);					// 反馈开关on数据
static void AckSwitchOff(void);					// 反馈开关off数据


/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/

/*********************************************************************************************************
* 函数名称：void PWM_CloseBright(void)
* 
* 函数功能：关闭照明
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 注    意：
* 修改记录：
*********************************************************************************************************/
static void PWM_CloseBright(void)
{
	g_strBlinkerData.wwBright = 0;
	PWM_Drive_PD4(0);
}

/*********************************************************************************************************
* 函数名称：void PWM_CloseColor(void)
* 
* 函数功能：关闭彩灯
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 注    意：
* 修改记录：
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
* 函数名称：void PWM_CloseAll(void)
* 
* 函数功能：PWM输出全部关闭
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 注    意：
* 修改记录：
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
* 函数名称：void PWM_SetBright(uint32_t brightness)
* 
* 函数功能：设置照明PWM输出
* 
* 输入参数：brightness = 照明亮度比例值(0 ~ 255)
* 
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 注    意：
* 修改记录：
*********************************************************************************************************/
void PWM_SetBright(uint32_t brightness)
{
	PWM_Drive_PD4((brightness * WW_PWM_PULSE) / LED_BRIGH_MAX);	
}

/*********************************************************************************************************
* 函数名称：void PWM_SetColor(uint32_t r_value, uint32_t g_value, uint32_t b_value, uint32_t bright_value)
* 
* 函数功能：设置七彩PWM输出
* 
* 输入参数：r_value, g_value, b_value = 红、绿、蓝光比例值(0 ~ 255)
*           bright_value = 亮度比例值(0 ~ 255)
* 
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 注    意：
* 修改记录：
*********************************************************************************************************/
void PWM_SetColor(uint32_t r_value, uint32_t g_value, uint32_t b_value, uint32_t bright_value)
{	
	PWM_Drive_PD1((b_value * B_PWM_PULSE * bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
	PWM_Drive_PD2((r_value * R_PWM_PULSE * bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
	PWM_Drive_PD3((g_value * G_PWM_PULSE * bright_value) / (LED_COLOR_MAX * LED_BRIGH_MAX));
}


/*********************************************************************************************************
* 函数名称：static void AckBrightData(void)
* 函数功能：反馈照明亮度数据
* 输入参数：
* 输出参数：
* 返 回 值：
* 创建日期：2018年01月01日
* 注    意：
* 修改记录：
*********************************************************************************************************/
static void AckBrightData(void)
{
	char	 sendBuf[64];
	int32_t  sendCount;

	// 照明数值：{"ran-ww":{"val":100}}
	sendCount = sprintf(sendBuf, "{\"ran-ww\":{\"val\":%u}}\r\n", g_strBlinkerData.wwBright);

	if (sendCount > 0)
	{
		UART_SendStringLen(sendBuf, (uint32_t)sendCount);
	}
}

/*********************************************************************************************************
* 函数名称：static void AckColorData(void)
* 函数功能：反馈彩灯亮度数据
* 输入参数：
* 输出参数：
* 返 回 值：
* 创建日期：2018年01月01日
* 注    意：
* 修改记录：
*********************************************************************************************************/
static void AckColorData(void)
{
	char	 sendBuf[64];
	int32_t  sendCount;
	
	// 七彩灯光: {"col-led":[R,G,B,Brightness]}
	sendCount = sprintf(sendBuf, "{\"col-led\":[%u,%u,%u,%u]}\r\n", g_strBlinkerData.r_value, g_strBlinkerData.g_value, g_strBlinkerData.b_value, g_strBlinkerData.bright_value);

	if (sendCount > 0)
	{
		UART_SendStringLen(sendBuf, (uint32_t)sendCount);
	}
}

/*********************************************************************************************************
* 函数名称：static void AckSwitchOn(void)
* 函数功能：反馈开关on数据
* 输入参数：
* 输出参数：
* 返 回 值：
* 创建日期：2018年01月01日
* 
* 注    意：中文信息必须使用UTF-8编码，使用\x输出，notepad++转换编码
* 
* 修改记录：
*********************************************************************************************************/
static void AckSwitchOn(void)
{
	// 开关状态: {"btn-abc":{"swi":"on","tex":"已开灯"}}
	UART_SendStringLen("{\"btn-abc\":{\"swi\":\"on\",\"tex\":\"\xe5\xb7\xb2\xe5\xbc\x80\xe7\x81\xaf\"}}\r\n", sizeof("{\"btn-abc\":{\"swi\":\"on\",\"tex\":\"\xe5\xb7\xb2\xe5\xbc\x80\xe7\x81\xaf\"}}\r\n") - 1);
}

/*********************************************************************************************************
* 函数名称：static void AckSwitchOff(void)
* 函数功能：反馈开关off数据
* 输入参数：
* 输出参数：
* 返 回 值：
* 创建日期：2018年01月01日
* 
* 注    意：中文信息必须使用UTF-8编码，使用\x输出，notepad++转换编码
* 
* 修改记录：
*********************************************************************************************************/
static void AckSwitchOff(void)
{
	// 开关状态: {"btn-abc":{"swi":"off","tex":"已关灯"}}  
	UART_SendStringLen("{\"btn-abc\":{\"swi\":\"off\",\"tex\":\"\xe5\xb7\xb2\xe5\x85\xb3\xe7\x81\xaf\"}}\r\n", sizeof("{\"btn-abc\":{\"swi\":\"off\",\"tex\":\"\xe5\xb7\xb2\xe5\x85\xb3\xe7\x81\xaf\"}}\r\n") - 1);
}

/*********************************************************************************************************
* 函数名称：static void AckHeartBeat(void)
* 函数功能：反馈心跳包数据
* 输入参数：
* 输出参数：
* 返 回 值：
* 创建日期：2018年01月01日
* 注    意： 
* 修改记录：
*********************************************************************************************************/
static void AckHeartBeat(void)
{
	// BLE设备: {"state":"connected"}
	UART_SendStringLen("{\"state\":\"connected\"}\r\n", sizeof("{\"state\":\"connected\"}\r\n") - 1);
}



/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：void BlinkerInitialize(void)
* 
* 函数功能：点灯科技APP接口初始化
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：
* 
* 修改记录：
*********************************************************************************************************/
void BlinkerInitialize(void)
{
	// 串口接收参数初始化
	g_strUartRecvBuffer.RecvStep  = 0;
	g_strUartRecvBuffer.RecvCount = 0;
	g_strUartRecvBuffer.MarkCount = 0;
	
	// 串口接收IRQ开启
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);

	// LED控制状态数据
	g_strBlinkerData.led_switch = 0;
	g_strBlinkerData.led_mode   = 0;
	g_strBlinkerData.iSwwChange = 0;
	g_strBlinkerData.HeartTick  = -1ul;

	// 提取EEPROM亮度值
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
* 函数名称：void BlinkerProcess(void)
* 
* 函数功能：点灯科技APP数据流处理
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：1.组件ID固定输入
*			2.必须循环，实时调用	!!!!!!!!!!!
* 
* 修改记录：
*********************************************************************************************************/
void BlinkerProcess(void)
{
	char     *pBuf;
	uint32_t sliderValue;
	uint32_t r_value, g_value, b_value, bright_value;
	
	// 检测串口接收否?
	if (UART_CheckReceive())
	{
		// 1.心跳与同步反馈: {"get":"state"}\n
		if (NULL != strstr((char *)g_strUartRecvBuffer.Buffer, "\"get\":\"state\""))
		{
			AckHeartBeat();
			g_strBlinkerData.HeartTick = g_u32_1msTick;

			// 同步设备状态
			if (0 == g_strBlinkerData.led_switch)
			{
				AckSwitchOff();
			} 
			else
			{
				AckSwitchOn();

				if (0 == g_strBlinkerData.led_mode)
				{
					// 照明数值
					AckBrightData();
				} 
				else
				{
					// 七彩灯光
					AckColorData();
				}
			}
		}
	
		// 2.开关按键: {"btn-abc":"on"}\n, {"btn-abc":"off"}\n
		if (NULL != strstr((char *)g_strUartRecvBuffer.Buffer, "\"btn-abc\":\"on\""))
		{
			g_strBlinkerData.led_switch = 1;
			AckSwitchOn();

			// 同步设备状态
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

		// 3.开关按键: {"btn-abc":"off"}
		if (NULL != strstr((char *)g_strUartRecvBuffer.Buffer, "\"btn-abc\":\"off\""))
		{
			g_strBlinkerData.led_switch = 0;
			AckSwitchOff();

			// 同步设备状态
			PWM_CloseAll();
		}
		
		// 4.滑动条组件: {"ran-ww":30}\n
		pBuf = strstr((char *)g_strUartRecvBuffer.Buffer, "\"ran-ww\":");
		if (NULL != pBuf)
		{
			sliderValue = 0;

			if (1 == sscanf(pBuf, "\"ran-ww\":%u", &sliderValue))
			{
				// 同步设备状态
				g_strBlinkerData.wwBright   = sliderValue;
				g_strBlinkerData.iSwwChange = 1;
				g_strBlinkerData.wwBrightTick = g_u32_1msTick;

				g_strBlinkerData.led_mode   = 0;
				g_strBlinkerData.led_switch = 1;
				AckSwitchOn();													//反馈开关状态为开
		
				PWM_CloseColor();												//关闭所有彩灯
				PWM_SetBright(g_strBlinkerData.wwBright);  				        //PWM占空比处理

				AckBrightData();												//避免滑动速度太快造成误差
			}
		}

		// 5.颜色组件: {"col-led":[R,G,B,Brightness]}\n
		pBuf = strstr((char *)g_strUartRecvBuffer.Buffer, "\"col-led\":[");
		if (NULL != pBuf)
		{
			r_value = 0;
			g_value = 0;
			b_value = 0;
			bright_value = 0;

			if (4 == sscanf(pBuf, "\"col-led\":[%u,%u,%u,%u]", &r_value, &g_value, &b_value, &bright_value))
			{
				// 同步设备状态
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
		
		// 处理完成，重启进程
		UART_InitReceive();
	}

	// 照明亮度保存
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
* 函数名称：void BlinkAdjustBright(void)
* 
* 函数功能：依据灯光状态，自动循环调整灯光亮度
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：1.调整范围0 ~ 255，步进25
* 
* 修改记录：
*********************************************************************************************************/
void BlinkAdjustBright(void)
{
	// 检测灯亮否？
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
	// 检测灯亮否？
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
* 函数名称：void BlinkToggleLed(void)
* 
* 函数功能：开关LED灯
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
void BlinkToggleLed(void)
{
//	static uint32_t ledState;
	if (0 == g_strBlinkerData.led_switch)
	{
		// 同步设备状态
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
		// 同步设备状态
		PWM_CloseAll();
		g_strBlinkerData.led_switch = 0;
		g_strBlinkerData.led_mode = 0;
		Ledchose = 0;
	
		AckSwitchOff();
	}
}
/*********************************************************************************************************
* 函数名称：void BlinkToggleLed(void)
* 
* 函数功能：开关LED灯
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
