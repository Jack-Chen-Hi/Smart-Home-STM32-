#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
  	
//IO��������
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}

//IO��������	 
#define IIC_SCL    PBout(6)                                                   //SCL
#define IIC_SDA    PBout(7)                                                   //SDA	 
#define READ_SDA   PBin(7)  
//����SDA 

//IIC���в�������
void BMP_IIC_Init(void);                                                           //��ʼ��IIC��IO��				 
void BMP_IIC_Start(void);				                                           //����IIC��ʼ�ź�
void BMP_IIC_Stop(void);	  			                                           //����IICֹͣ�ź�
void BMP_IIC_Send_Byte(u8 txd);			                                           //IIC����һ���ֽ�
u8 BMP_IIC_Read_Byte(unsigned char ack);                                           //IIC��ȡһ���ֽ�
u8 BMP_IIC_Wait_Ack(void); 				                                           //IIC�ȴ�ACK�ź�
void BMP_IIC_Ack(void);					                                           //IIC����ACK�ź�
void BMP_IIC_NAck(void);				                                           //IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	
			 
u8 iicDevReadByte(u8 devaddr,u8 addr);	                                       /*��һ�ֽ�*/
void iicDevWriteByte(u8 devaddr,u8 addr,u8 data);	                           /*дһ�ֽ�*/
void iicDevRead(u8 devaddr,u8 addr,u8 len,u8 *rbuf);                           /*������ȡ����ֽ�*/
void iicDevWrite(u8 devaddr,u8 addr,u8 len,u8 *wbuf);                          /*����д�����ֽ�*/

#endif

