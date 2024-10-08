#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
 //蜂鸣器 IO口设置
#define BEEP_PIN         GPIO_Pin_8
#define BEEP_GPIO        GPIOA
#define BEEP_SET         GPIO_SetBits(BEEP_GPIO , BEEP_PIN)  //高电平 蜂鸣器响
#define BEEP_RESET       GPIO_ResetBits(BEEP_GPIO , BEEP_PIN) //低电平  蜂鸣器关闭
#define BEEP PAout(8)// PA8	
void BEEP_Init(void);//IO初始化
				    
#endif
