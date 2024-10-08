#include "motor.h"
void Motor_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12; //端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
 GPIO_ResetBits(GPIOB,GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12);	//输出低
}
 
void BUJING_Cotrol(u8 mode,u16 time,u16 count)
{
	if(mode==0)
	{
		while(count--)
		{
			BUJ1=1; BUJ2=0; BUJ3=0;BUJ4=0;
			delay_ms(time);
			BUJ1=0; BUJ2=1; BUJ3=0;BUJ4=0;
			delay_ms(time);
			BUJ1=0; BUJ2=0; BUJ3=1;BUJ4=0;
			delay_ms(time);
			BUJ1=0; BUJ2=0; BUJ3=0;BUJ4=1;
			delay_ms(time);
		}
	}
	if(mode==1)
	{
		while(count--)
		{
			BUJ1=0; BUJ2=0; BUJ3=0;BUJ4=1;
			delay_ms(time);
			BUJ1=0; BUJ2=0; BUJ3=1;BUJ4=0;
			delay_ms(time);
			BUJ1=0; BUJ2=1; BUJ3=0;BUJ4=0;
			delay_ms(time);
			BUJ1=1; BUJ2=0; BUJ3=0;BUJ4=0;
			delay_ms(time);
		}
	}
}
