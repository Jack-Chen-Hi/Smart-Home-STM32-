#ifndef __TIMER_H
#define __TIMER_H
#include "include.h"

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM2_PWM_Init(u16 arr,u16 psc);
void TIM4_PWM_Init(u16 arr,u16 psc);
void TIM7_Int_Init(u16 arr,u16 psc);
#endif

//������ƻ������ݣ�S-D5�ͺţ�
/*{
u16 djpwmval=0;
TIM3_PWM_Init(39999,35);	 //��Ƶϵ��35������20ms
djpwmval=34900;          //�����ֵ36500 �����ֵ38400��Ԥ��50���壩 �����ֵ34900���޻���Ԥ����
TIM_SetCompare2(TIM3,djpwmval);		//����ccr�Ĵ���ֵʱ����ߵ�ƽ
}*/
/*******************************************************/
/*{
//			 if(dir==1)  djpwmval+=5;
//			 if(dir==0)  djpwmval-=5;
//			 if(djpwmval>=38400) dir=0;
//			 if(djpwmval<=34900) dir=1;
//		   TIM_SetCompare3(TIM3,djpwmval);	
}*/








