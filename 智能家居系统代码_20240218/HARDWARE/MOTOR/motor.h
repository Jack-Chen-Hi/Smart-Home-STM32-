#ifndef __MOTOR_H
#define __MOTOR_H	 
#include "sys.h"
#define BUJ1 PBout(15)// PB15
#define BUJ2 PBout(14)// PB14
#define BUJ3 PBout(13)// PB13
#define BUJ4 PBout(12)// PB12
void Motor_Init(void);//≥ı ºªØ
void BUJING_Cotrol(u8 mode,u16 time,u16 count);		 				    
#endif
