#ifndef __LED_H
#define __LED_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	




#define INH PFout(1)	// DS0
#define A PFout(3)	// DS1	
#define B PFout(5)	// DS1	
#define C PFout(7)	// DS1	
#define LED0 PFout(9)	// DS0
#define LED1 PFout(10)	// DS1	 
void LED_Init(void);//��ʼ��		 				    
#endif
