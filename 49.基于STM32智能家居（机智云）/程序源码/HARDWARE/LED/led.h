#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PBout(0)// PB5
#define BEEP PCout(13)// PE5	

#define BUJ1 PAout(11)// PE5	
#define BUJ2 PAout(12)// PE5	
#define BUJ3 PAout(15)// PE5	
#define BUJ4 PBout(3)// PE5	
void LED_Init(void);//��ʼ��

		 				    
#endif
