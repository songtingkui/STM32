/*********************************************************************************************************
* 模块名称：delay.c
* 
* 摘    要：延时函数库（使用指令延时）
* 
* 当前版本：1.0.0
* 
* 作    者：ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* 完成日期：2021年05月11日 
* 
* 内    容：毫秒级延时
*			
* 注    意：
*     
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
#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/


/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void SysTick_Initialize(void);
extern uint32_t g_u32_1msTick;

#ifdef  __cplusplus
}
#endif

#endif  /* __DELAY_H */
