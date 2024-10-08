#include "stm32f10x.h"
#include "fan.h"
#include "sys.h" 
void FcInit(void) //风扇函数
{
  GPIO_InitTypeDef  GPIO_InitStructure; //定义结构体变量
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA时钟
	GPIO_InitStructure.GPIO_Pin = FC_PIN; //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//配置GPIO端口速度
	GPIO_Init(FC_GPIO , &GPIO_InitStructure); //初始化GPIO
	FC_RESET;	//低电平，关风扇
}								    
