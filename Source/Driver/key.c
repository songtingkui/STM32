/*********************************************************************************************************
* 模块名称：key.c
* 
* 摘    要：板载独立按键控制接口（PA1，低有效）
* 
* 当前版本：1.0.0
* 
* 作    者：ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* 完成日期：2021年05月11日 
* 
* 内    容：1、接口初始化 - 上拉
*			2、按键识别 - PA1，低有效
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

#include "key.h"
#include "delay.h"
#include "blinker.h"
/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define KEY_JIT_TIME		50									// 按键去抖动延时时间(同状态持续时间)ms
#define KEY_LONG_TIME		1500								// 长按键识别时间ms
#define KEY_SHORT_TIME		100								// 短按键识别时间ms
#define KEY_PWRON_STATUS	Bit_SET								// 按键上电默认状态

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static uint8_t  KeyShortStatus;									// 有按键动作（按下 & 释放）否：1 - 按键，0 - 没有
static uint8_t  KeyLongStatus;									// 有长按键动作否：1 - 按键，0 - 没有

extern uint8_t MusicVolumeUp[4];		//音量加
extern uint8_t MusicVolumeDown[4];		//音量减
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
* 函数名称：void Key_Init(void)
* 
* 函数功能：独立按键接口初始化
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
void Key_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// Clock
	RCC_AHBPeriphClockCmd(MAIN_KEY_CLK, ENABLE);
	
	// GPIO
    GPIO_InitStructure.GPIO_Pin  = MAIN_KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Enable;
    GPIO_Init(MAIN_KEY_PORT, &GPIO_InitStructure);
	
}

/*********************************************************************************************************
* 函数名称：void ScanKeyboard1(void)
* 
* 函数功能：扫描按键状态变化
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：需要实时调用，否则识别不了按键变化	!!!!!!!!!!!
* 
* 修改记录：
*********************************************************************************************************/
void ScanKeyboard1(void)
{
	static uint32_t Keysteps = 1;	
	static uint32_t KeyTick  = 1;
	switch(Keysteps)
	{
		case 1:
			//检查按下否
			if(0 == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
			{
				KeyTick = g_u32_1msTick;
				Keysteps = 2;
			}
		
		break;
			
		case 2:
			//去抖动等待
			if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_JIT_TIME)
			{
				Keysteps = 3;
			}
		
		
		break;
		
		case 3:
			//再次确定是否按键
			if(0 == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
			{
				Keysteps = 4;
			}
			else
			{
				Keysteps = 1;
			}
		break;
		
		case 4:
			
		if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_SHORT_TIME)
		{
			//短按键程序
			GPIO_SetBits(GPIOC, GPIO_Pin_7);	
			
			
			
			Keysteps = 5;
		}
			
		break;
		
		case 5:
		if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_LONG_TIME)
		{
			//长按键程序
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
			KeyLongStatus = 1;
			
			Keysteps = 6;
		}
		else if(0 != GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
		{
			Keysteps = 1;	
			KeyShortStatus = 1; 
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);			
		}	
		break;
		
		case 6:
		
		//等待按键释放
		if(0 != GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
		{
			Keysteps = 1;	
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		}	
		break;
	}
	
}	

/*********************************************************************************************************
* 函数名称：void ScanKeyboard1(void)
* 
* 函数功能：扫描按键状态变化
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：需要实时调用，否则识别不了按键变化	!!!!!!!!!!!
* 
* 修改记录：
*********************************************************************************************************/
void ScanKeyboard2(void)
{
	static uint32_t Keysteps = 1;	
	static uint32_t KeyTick  = 1;
	switch(Keysteps)
	{
		case 1:
			//检查按下否
			if(0 == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))
			{
				KeyTick = g_u32_1msTick;
				Keysteps = 2;
			}
		
		break;
			
		case 2:
			//去抖动等待
			if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_JIT_TIME)
			{
				Keysteps = 3;
			}
		
		
		break;
		
		case 3:
			//再次确定是否按键
			if(0 == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))
			{
				Keysteps = 4;
			}
			else
			{
				Keysteps = 1;
			}
		break;
		
		case 4:
			
		if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_SHORT_TIME)
		{
			//短按键程序
			GPIO_SetBits(GPIOC, GPIO_Pin_7);
			KeyShortStatus = 2;    
			
			
			Keysteps = 5;
		}
			
		break;
		
		case 5:
		if((uint32_t)(g_u32_1msTick - KeyTick) >= KEY_LONG_TIME)
		{
			//长按键程序
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
			KeyLongStatus = 2;
			
			Keysteps = 6;
		}
		else if(0 != GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))
		{
			Keysteps = 1;	
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		}	
		break;
		
		case 6:
		
		//等待按键释放
		if(0 != GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))
		{
			Keysteps = 1;	
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		}	
		break;
	}
	
}

/*********************************************************************************************************
* 函数名称：void sysKeyProcess(void)
* 
* 函数功能：LED指示灯接口初始化
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
void sysKeyProcess(void)
{
	// KEY_PA1检测短按键否？
	if (1 == KeyShortStatus)
	{
		KeyShortStatus = 0;
		
		// 亮灯状态下调整亮度->调亮
		BlinkAdjustBright();		
	}
	
	// KEY_PA1检测长按键否？
	if (1 == KeyLongStatus)
	{
		KeyLongStatus = 0;
		
		// 切换灯开关状态-> 开 关
		BlinkToggleLed();	
	}
	
	// KEY_PA2检测短按键否？
	if (2 == KeyShortStatus)
	{
		KeyShortStatus = 0;
		
		// 亮灯状态下调整亮度->调暗
		BlinkAdjustBright2();		
	}
	
	// KEY_PA2按键检测长按键否？
	if (2 == KeyLongStatus)
	{
		KeyLongStatus = 0;
		
		// 切换彩灯灯状态
		 BlinkColorchose();
		
	}
}
