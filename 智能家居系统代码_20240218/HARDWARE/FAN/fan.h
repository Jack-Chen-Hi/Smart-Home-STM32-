#ifndef __FAN_H
#define __FAN_H	 
#include "sys.h"
//FC IO������
#define FC_PIN         GPIO_Pin_11
#define FC_GPIO        GPIOA
#define FC_SET         GPIO_SetBits(FC_GPIO , FC_PIN)  //�ߵ�ƽ
#define FC_RESET       GPIO_ResetBits(FC_GPIO , FC_PIN) //�͵�ƽ
void FcInit(void);				    
#endif

