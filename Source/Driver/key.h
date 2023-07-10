/*********************************************************************************************************
* ģ�����ƣ�key.h
* 
* ժ    Ҫ�����ض����������ƽӿڣ�PA1������Ч��
* 
* ��ǰ�汾��1.0.0
* 
* ��    �ߣ�ZLGCX(COPYRIGHT 2021 - 2030 ZLGCX. All rights reserved.)
* 
* ������ڣ�2021��05��11�� 
* 
* ��    �ݣ�1���ӿڳ�ʼ�� - ����
*			2������ʶ�� - PA1������Ч
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

#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C" {
#endif
	
	
/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "hk32f030m.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define MAIN_KEY_PORT		GPIOA						// ���������˿�
#define MAIN_KEY_PIN		GPIO_Pin_1 | GPIO_Pin_2
#define MAIN_KEY_CLK		RCC_AHBPeriph_GPIOA

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
	
/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void Key_Initialize(void);								// �����ӿڳ�ʼ��

void ScanKeyboard1(void);								// ɨ�谴��״̬�仯->1
void ScanKeyboard2(void);								// ɨ�谴��״̬�仯->2
void sysKeyProcess(void);
////uint32_t Key_IsPress(void);								// ����ѹ�·�
////uint32_t Key_IsLongPress(void);							// ������ѹ�·�
////uint32_t Key_IsRelease(void);							// �����ͷŷ�
////uint32_t Key_IsTap(void);								// �Ƿ��а���������
	

#ifdef  __cplusplus
}
#endif

#endif  /* __KEY_H */