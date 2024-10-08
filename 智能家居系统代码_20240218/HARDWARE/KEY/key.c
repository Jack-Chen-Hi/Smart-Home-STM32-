#include "stm32f10x.h"
#include "key.h"
#include "sys.h"  
								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTB时钟
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//配置GPIO端口速度
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//KEY1-KEY4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY1按下
//2，KEY2按下
//3，KEY3按下 
//4，KEY4按下
//注意此函数有响应优先级,KEY1>KEY2>KEY3>KEY4!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==0||GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)==0||GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==0||GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==0)return KEY1_PRES;
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)==0)return KEY2_PRES;
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==0)return KEY3_PRES;
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==0)return KEY4_PRES;
	}else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==1&&GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)==1&&GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==1&&GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==1)key_up=1; 	    
 	return 0;// 无按键按下
}
