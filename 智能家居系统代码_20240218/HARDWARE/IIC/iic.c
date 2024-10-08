//////////////////////////////////////////////////////////////////////////////////	 
//  �� �� ��   : oled.c
//  �� �� ��   : v2.0
//  ��    ��   : ZX
//  ��������   : 2022-0101
//  ����޸�   : 
//  ��������   : 0.96��OLED �ӿ���ʾ����(STM32F103ZEϵ��IIC)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND   ��Դ��
//              VCC   ��5V��3.3v��Դ
//              SCL   ��PB10��SCL��
//              SDA   ��PB11��SDA��            
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////

#include "iic.h"
#include "stdlib.h"
void IIC_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
	SDA_OUT();
	I2C_SDA_1();
	I2C_SCL_1();
	I2C_SDA_0();
	I2C_SCL_0();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	SDA_OUT(); 
	I2C_SDA_0();
	I2C_SCL_1();
	I2C_SDA_1();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Send_Byte(uint8_t _ucByte)
{
	uint8_t i;
	SDA_OUT(); 
	/* �ȷ����ֽڵĸ�λbit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		I2C_SCL_1();
		I2C_SCL_0();
		if (i == 7)
		{
			 I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t IIC_Read_Byte(u8 ack)
{
	uint8_t i;
	uint8_t value;
	SDA_IN();
	/* ������1��bitΪ���ݵ�bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
	}
	if (!ack)
        IIC_NAck();//??nACK
    else
        IIC_Ack(); //??ACK  
	return value;
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime;
	SDA_IN();  
	I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	while (I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	I2C_SCL_0();
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Ack(void)
{
	SDA_OUT();
	I2C_SDA_0();	/* CPU����SDA = 0 */
	I2C_SCL_1();	/* CPU����1��ʱ�� */
	I2C_SCL_0();
	I2C_SDA_1();	/* CPU�ͷ�SDA���� */
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_NAck(void)
{
	SDA_OUT();
	I2C_SDA_1();	/* CPU����SDA = 1 */
	I2C_SCL_1();	/* CPU����1��ʱ�� */
	I2C_SCL_0();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_CfgGpio
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_I2C_PORT, ENABLE);	/* ��GPIOʱ�� */
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* ��©��� */
	GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);
	/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
	IIC_Stop();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	IIC_Init();		/* ����GPIO */

	
	IIC_Start();		/* ���������ź� */

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	IIC_Send_Byte(_Address | I2C_WR);
	ucAck = IIC_Wait_Ack();	/* ����豸��ACKӦ�� */

	IIC_Stop();			/* ����ֹͣ�ź� */

	return ucAck;
}


//��ָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 iicDevReadByte(u8 devaddr,u8 addr)
{				  
	u8 temp=0;		  	    																 
	IIC_Start();  
	IIC_Send_Byte(devaddr);                                                    //��������д���� 	   
	IIC_Wait_Ack(); 
	IIC_Send_Byte(addr);                                                       //���͵͵�ַ
	IIC_Wait_Ack();	

	IIC_Start();  	 	   
	IIC_Send_Byte(devaddr|1);                                                  //��������������			   
	IIC_Wait_Ack();	 
	temp=IIC_Read_Byte(0);			   
	IIC_Stop();                                                                //����һ��ֹͣ����	    
	return temp;
}
//����������ֽ�
//addr:��ʼ��ַ
//rbuf:�����ݻ���
//len:���ݳ���
void iicDevRead(u8 devaddr,u8 addr,u8 len,u8 *rbuf)
{
	int i=0;
	IIC_Start();  
	IIC_Send_Byte(devaddr);  
	IIC_Wait_Ack();	
	IIC_Send_Byte(addr);                                                       //��ַ����  
	IIC_Wait_Ack();	

	IIC_Start();  	
	IIC_Send_Byte(devaddr|1);  	
	IIC_Wait_Ack();	
	for(i=0; i<len; i++)
	{
		if(i==len-1)
		{
			rbuf[i]=IIC_Read_Byte(0);                                          //���һ���ֽڲ�Ӧ��
		}
		else
			rbuf[i]=IIC_Read_Byte(1);
	}
	IIC_Stop( );	
}

//��ָ����ַд��һ������
//WriteAddr :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void iicDevWriteByte(u8 devaddr,u8 addr,u8 data)
{				   	  	    																 
	IIC_Start();  
	IIC_Send_Byte(devaddr);                                                    //��������д���� 	 
	IIC_Wait_Ack();	   
	IIC_Send_Byte(addr);                                                       //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(data);                                                       //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();		                                                           //����һ��ֹͣ���� 	 
}

//����д����ֽ�
//addr:��ʼ��ַ
//wbuf:д���ݻ���
//len:���ݵĳ���
void iicDevWrite(u8 devaddr,u8 addr,u8 len,u8 *wbuf)
{
	int i=0;
	IIC_Start();  
	IIC_Send_Byte(devaddr);  	
	IIC_Wait_Ack();	
	IIC_Send_Byte(addr);  //��ַ����
	IIC_Wait_Ack();	
	for(i=0; i<len; i++)
	{
		IIC_Send_Byte(wbuf[i]);  
		IIC_Wait_Ack();		
	}
	IIC_Stop( );	
}


