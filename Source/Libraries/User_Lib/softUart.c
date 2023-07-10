/*********************************************************************************************************
* 模块名称：softUart.c
* 
* 摘    要：IO模拟串口库
* 
* 当前版本：1.0.0
* 
* 作    者：ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* 完成日期：2021年05月11日 
* 
* 内    容：1、softUart初始化
*			2、softUart数据收发
*			
* 注    意：1、本软串口库采用阻塞收发模式，仅供半双工通信，收发分时
*			2、发送期间使用Systick定时器，此时不能它用；
*			3、接收使用Timer1，不能它用；接收外部中断开启，修改IO，需要调整中断
*			4、接收数据可以外部查询 或 中断接收
*			5、波特率设置建议是硬件串口的3倍左右，防止本串口的死收发时间造成硬件串口漏收数据
*                                                                 
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

#include "softUart.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define SOFT_TX_PORT		GPIOC												// 软串口TX端口
#define SOFT_TX_PIN			GPIO_Pin_5
#define SOFT_TX_CLK			RCC_AHBPeriph_GPIOC

#define SOFT_RX_PORT		GPIOC												// 软串口RX端口: 修改本端口后注意修改外部中断代码
#define SOFT_RX_PIN			GPIO_Pin_6
#define SOFT_RX_CLK			RCC_AHBPeriph_GPIOC

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static uint8_t  s_u8SoftUartRecvData, s_u8SoftUartHaveData;
static uint32_t s_u32SoftUartBaudTick;

//extern uint16_t comCount;														// 中断接收测试
//extern uint8_t comBuffer[256];
extern uint32_t g_u8ComRecvSoft;
extern uint32_t g_u32ComRecv_mark;
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
* 函数名称：void softUart_init(uint32_t baudRate)
* 
* 函数功能：初始化软串口IO 及 接收定时器 和 外部中断
* 
* 输入参数：baudRate = 串口波特率，最大：57600；其他固定：8, n, 1
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：
* 修改记录：
*********************************************************************************************************/
void softUart_init(uint32_t baudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	// 波特率换算为延时时钟数（主时钟，不分频）
	s_u8SoftUartHaveData  = 0;
	s_u8SoftUartRecvData  = 0;
	s_u32SoftUartBaudTick = SystemCoreClock / baudRate - 1;
	
	// TX
	RCC_AHBPeriphClockCmd(SOFT_TX_CLK, ENABLE);
    GPIO_SetBits(SOFT_TX_PORT, SOFT_TX_PIN);					//PC5
	
    GPIO_InitStructure.GPIO_Pin  = SOFT_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
    GPIO_Init(SOFT_TX_PORT, &GPIO_InitStructure);
	
	// RX
	RCC_AHBPeriphClockCmd(SOFT_RX_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = SOFT_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Enable;
	GPIO_Init(SOFT_RX_PORT, &GPIO_InitStructure);

	// 数据bit接收时钟: TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = s_u32SoftUartBaudTick;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	// RX: EXTI
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);
	EXTI_ClearITPendingBit(EXTI_Line6);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*********************************************************************************************************
* 函数名称：void EXTI3_IRQHandler(void)
* 
* 函数功能：软串口RX: 外部中断程序
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：1、使用本地全局变量记录接收数据
*			2、中断接收数据，在“用户中断接收处理”
*			3、采用阻塞接收，会影响串口发送
* 
* 修改记录：
*********************************************************************************************************/
void EXTI6_IRQHandler(void)
{
	uint32_t i, dat;
  
	__disable_irq();
	if (EXTI_GetFlagStatus(EXTI_Line6) != RESET)
	{
		// TIM1 enable counter
		TIM_SetCounter(TIM1, 0);
		TIM_ClearFlag(TIM1, TIM_FLAG_Update);
		TIM_Cmd(TIM1, ENABLE);	  
	
		// 8bit接收
		dat = 0;
		for (i = 0; i < 8; i++)
		{
			// 等待bit间隔时间
			while (RESET == TIM_GetFlagStatus(TIM1, TIM_FLAG_Update));
			TIM_ClearFlag(TIM1, TIM_FLAG_Update);
			
			// 接收1bit，D7 -> D0
			dat >>= 1;
			if (RESET != GPIO_ReadInputDataBit(SOFT_RX_PORT, SOFT_RX_PIN))
			{
				dat |= 0x80;
			}
		}

		// 8bit接收完毕：清除中断位
		TIM_Cmd(TIM1, DISABLE);	  
		EXTI_ClearITPendingBit(EXTI_Line6);
		
		// 输出数据及标志
		s_u8SoftUartRecvData = dat;
		s_u8SoftUartHaveData = 1;

		// 用户中断接收处理
		g_u8ComRecvSoft = s_u8SoftUartRecvData;
		g_u32ComRecv_mark = 1;
//		comBuffer[comCount] = s_u8SoftUartRecvData;
//		comCount++;
	}
	__enable_irq();
}

/*********************************************************************************************************
* 函数名称：void softUart_SendChar(uint8_t ch)
* 
* 函数功能：软串口数据发送（8bit，阻塞）
* 
* 输入参数：ch = 要传输的数据
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：才能阻塞发送，使用SysTick定时器，不分频
* 
* 修改记录：
*********************************************************************************************************/
void softUart_SendChar(uint8_t ch)
{
	uint32_t i, sysTickCtrlBak;
	
	// 清除Systick计数标志
	sysTickCtrlBak= SysTick->CTRL;

	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;	
	SysTick->VAL  = 0;
	SysTick->LOAD = s_u32SoftUartBaudTick;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;	
	
	// 起始位
	__disable_irq();
    GPIO_ResetBits(SOFT_TX_PORT, SOFT_TX_PIN);

	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL &= (~SysTick_CTRL_COUNTFLAG_Msk);
	
	// 8bit数据, D0 -> D7
	for (i = 0; i < 8; i++)
	{
		if (0 != (ch & 0x01))
		{
			GPIO_SetBits(SOFT_TX_PORT, SOFT_TX_PIN);
		}
		else
		{		
			GPIO_ResetBits(SOFT_TX_PORT, SOFT_TX_PIN);
		}
		ch >>= 1;

		while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
		SysTick->CTRL &= (~SysTick_CTRL_COUNTFLAG_Msk);
	}
	
	// 停止位
    GPIO_SetBits(SOFT_TX_PORT, SOFT_TX_PIN);

	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL &= (~SysTick_CTRL_COUNTFLAG_Msk);

	// 关闭计数器
	__enable_irq();
	SysTick->CTRL = sysTickCtrlBak;
}

/*********************************************************************************************************
* 函数名称：uint32_t softUart_CheckRecv(void)
* 
* 函数功能：查询是否有接收数据
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：0 - 无， 其他 - 有
* 
* 创建日期：2021年05月10日
* 
* 注    意：接收有数据标志，读数据时自动清零
* 
* 修改记录：
*********************************************************************************************************/
uint32_t softUart_CheckRecv(void)
{
	return s_u8SoftUartHaveData;
}

/*********************************************************************************************************
* 函数名称：uint32_t softUart_GetChar(void)
* 
* 函数功能：提取串口接收数据
* 
* 输入参数：None
* 输出参数：None
* 返 回 值：-1 = 串口无数据，其他 - 本次接收的有效数据
* 
* 创建日期：2021年05月10日
* 
* 注    意：
* 
* 修改记录：必须事先查询接收标志
* 
*********************************************************************************************************/
uint32_t softUart_GetChar(void)
{
	// 检测接收数据标志
	if (s_u8SoftUartHaveData)
	{
		// 自动清除标志
		s_u8SoftUartHaveData = 0;

		return s_u8SoftUartRecvData;
	} 
	else
	{
		// 无效数据
		return -1UL;
	}
}

/////////////////////////////////////////////////////////

void TIM6_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;          		   	        //声明TIM_TimeBaseInitTypeDef结构变量
	NVIC_InitTypeDef NVIC_InitStructure;                                        //声明NVIC_InitTypeDef 向量结构变量

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); 				        //使能TIM3时钟

	// 定时器基础设置
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;                      		    //设置自动重载计数=1199+1=1200

	TIM_TimeBaseStructure.TIM_Prescaler = 32 - 1;                	 		    //定时器的时钟频率=TIM_Prescaler+1=2；
	TIM_TimeBaseStructure.TIM_ClockDivision = 1;               			        //输入捕获时滤波用的参数,必须设一下
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  		        //向上计数
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);              		        //根据指定的参数初始

	// TIM6 中断嵌套设置

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;         		            //选择TIM3的IRQ通道
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;             	            //IRQ通道优先级=0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         		       	    //使能IRQ通道
	NVIC_Init(&NVIC_InitStructure);                                             //按以参数上设置

	TIM_ClearFlag(TIM6, TIM_FLAG_Update);                        	            //清除标志位，以免一启用中断后立即产生中断
	TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);                    		        //使能TIM3溢出中断
	
	TIM_Cmd(TIM6, ENABLE);                                                      //使能TIM3 总中断

}
//////////////////////////////////////////


///////////////////////////////////////////////

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
void LED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;						//变量定义
	
	//Clocks使能
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);    		
	
	// GPIO：PA2
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Enable;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_2);						//灭	
//////////////////////////////////////////////////////////////////////////
}


/*********************************************************************************************************
* 函数名称：void Ledshow(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
* 
* 函数功能：LED指示灯闪烁
* 
* 输入参数：GPIO_TypeDef* GPIOx 端口ABCD
*			uint16_t GPIO_Pin	端口1-7
* 输出参数：None
* 返 回 值：None
* 
* 创建日期：2021年05月10日
* 
* 注    意：
* 修改记录：
*********************************************************************************************************/
//uint32_t LedStep = 1;
//uint32_t LedTick = 0;

//void Ledshow(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
//{
//	//PC3	
//	switch (LedStep)
//		{
//			case 1:
//				// 1.亮
//				GPIO_ResetBits(GPIOx, GPIO_Pin);
//			
//				LedTick = g_u32_1msTick;
//				LedStep = 2;			
//			break;
//				
//			case 2:
//				// 2.等待		
//				if ((uint32_t)(g_u32_1msTick - LedTick) >= 500 )
//				{	
//					LedStep = 3;
//				}
//				break;
//			
//			case 3:
//				// 3.灭
//				GPIO_SetBits(GPIOx, GPIO_Pin);
//				LedTick = g_u32_1msTick;
//				LedStep = 4;			
//			
//			break;
//			
//			case 4:
//				// 4.等待
//				if ((uint32_t)(g_u32_1msTick - LedTick) >= 500)
//				{		
//					LedStep = 1;
//				}
//			break;
//		}		
//}











