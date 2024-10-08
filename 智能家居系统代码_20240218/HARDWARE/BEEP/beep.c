#include "stm32f10x.h"
#include "beep.h"
#include "sys.h" 
								    
void BEEP_Init(void) //����������
{
  GPIO_InitTypeDef  GPIO_InitStructure; //����ṹ�����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTAʱ��
	GPIO_InitStructure.GPIO_Pin = BEEP_PIN; //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����GPIO�˿��ٶ�
	GPIO_Init(BEEP_GPIO , &GPIO_InitStructure); //��ʼ��GPIO
	BEEP_RESET;	//�͵�ƽ���ط�����
}

