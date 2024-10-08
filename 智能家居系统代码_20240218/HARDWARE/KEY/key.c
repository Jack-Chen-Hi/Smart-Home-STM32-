#include "stm32f10x.h"
#include "key.h"
#include "sys.h"  
								    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTBʱ��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����GPIO�˿��ٶ�
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//KEY1-KEY4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY1����
//2��KEY2����
//3��KEY3���� 
//4��KEY4����
//ע��˺�������Ӧ���ȼ�,KEY1>KEY2>KEY3>KEY4!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==0||GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)==0||GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==0||GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==0)return KEY1_PRES;
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)==0)return KEY2_PRES;
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==0)return KEY3_PRES;
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==0)return KEY4_PRES;
	}else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==1&&GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)==1&&GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==1&&GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==1)key_up=1; 	    
 	return 0;// �ް�������
}
