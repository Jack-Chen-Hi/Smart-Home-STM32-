#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
 //������ IO������
#define BEEP_PIN         GPIO_Pin_8
#define BEEP_GPIO        GPIOA
#define BEEP_SET         GPIO_SetBits(BEEP_GPIO , BEEP_PIN)  //�ߵ�ƽ ��������
#define BEEP_RESET       GPIO_ResetBits(BEEP_GPIO , BEEP_PIN) //�͵�ƽ  �������ر�
#define BEEP PAout(8)// PA8	
void BEEP_Init(void);//IO��ʼ��
				    
#endif
