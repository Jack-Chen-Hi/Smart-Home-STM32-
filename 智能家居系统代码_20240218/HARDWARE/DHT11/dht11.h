#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
									  
//////////////////////////////////////////////////////////////////////////////////

#define DHT11_IO    GPIOA
#define DHT11_PIN   GPIO_Pin_12
//IO��������
#define DHT11_IO_IN()  {DHT11_IO->CRH&=0XFFF0FFFF;DHT11_IO->CRH|=8<<16;}
#define DHT11_IO_OUT() {DHT11_IO->CRH&=0XFFF0FFFF;DHT11_IO->CRH|=3<<16;}
////IO��������											   
#define	DHT11_DQ_OUT PAout(12) //���ݶ˿�	PA12 
#define	DHT11_DQ_IN  PAin(12)  //���ݶ˿�	PA12 


u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    
#endif















