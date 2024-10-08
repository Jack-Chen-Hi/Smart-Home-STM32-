#ifndef __FAN_H
#define __FAN_H	 
#include "sys.h"
//FC IO口设置
#define FC_PIN         GPIO_Pin_11
#define FC_GPIO        GPIOA
#define FC_SET         GPIO_SetBits(FC_GPIO , FC_PIN)  //高电平
#define FC_RESET       GPIO_ResetBits(FC_GPIO , FC_PIN) //低电平
void FcInit(void);				    
#endif

