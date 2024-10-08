#include "stm32f10x_it.h"
#include "sys.h"
#include "stdio.h" 
#include "iic.h"
#include "oled.h"
#include "bmp280.h"
#include "usart2.h"
#include "usart1.h"
#include "ADC.h"
#include "fan.h"
#include "beep.h"
#include "dht11.h"
#include "led.h"
#include "motor.h"
#include "key.h"
#include "timer.h"
#include "stmflash.h" 
#include "gizwits_product.h"
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
//***************************���þֲ�����***************************//
#define FLASH_SAVE_ADDR  0X0800E000	//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)
u8 buff[30];//������ʾ��������
u8 count;
u8 wifi_sta;//wifi����״̬��־
u8 mode=0;//ģʽ�����Զ����ֶ�
u8 NTP_sta=0;//��ŵʱ���ȡ״̬
u8 display_contrl=0;
u8 curtain_flag=0;
u8 last_curtain_flag=0;
char ctrl_comm = 1;//����ָ��
char ctrl_comm_last = 0;//��һ�ε�ָ��
extern u8 DHT11_Temp,DHT11_Hum;		//��ʪ��
u16 Pre;							//��ѹ���ֵ
u16 gz_value;						//���ռ��ֵ			
u16 m2_value;						//������ֵ
u16 m7_value;						//һ����̼���ֵ
u16 m135_value;						//�����������ֵ
u16 A_flag;				     //��־��Ϣ����
u16 A_DHT11_Temp;				//�¶���ֵ����
u16 A_DHT11_Hum;					//ʪ����ֵ����
u16 A_pre;						//��ѹ��ֵ����
u16 A_gz_value;				//����ǿ����ֵ����
u16 A_m2_value;				//������ֵ����
u16 A_m7_value;				//һ����̼��ֵ����
u16 A_m135_value;				//����������ֵ����
//*****************************************************************//
void Get_Data(u16 count);//��ȡ����������
void WIFI_Contection(u8 key);//WiFi���ӿ���
void Canshu_Change(u8 key);//ϵͳ��������
void Mode_Change(u8 key);//ģʽ�л�
void BUJING_Cotrol(u8 mode,u16 time,u16 count);
void Gizwits_Init(void);
int main(void)
{	
	u8 t=0;
	int key_value;		//����ֵ
	delay_init();	    //��ʱ������ʼ��
	STMFLASH_Read(FLASH_SAVE_ADDR+8,&A_flag,2);	//������ȡ��־λ
	if(A_flag == 65535)
	{
			A_flag=5555;
			A_DHT11_Temp=30;				//�¶���ֵ>30
			A_DHT11_Hum=35;					//ʪ����ֵ<35
			A_pre=1500;						  //��ѹ��ֵ1
			A_gz_value=1500;				//����ǿ����ֵ
			A_m2_value=1000;				//������ֵ>
			A_m7_value=1000;				//һ����̼��ֵ>
			A_m135_value=2800;				//����������ֵ<
			STMFLASH_Write(FLASH_SAVE_ADDR+8,&A_flag,2);
			STMFLASH_Write(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//�洢�¶���ֵ
			STMFLASH_Write(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//�洢ʪ����ֵ
			STMFLASH_Write(FLASH_SAVE_ADDR+14,&A_pre,2);			//�洢��ѹ��ֵ
			STMFLASH_Write(FLASH_SAVE_ADDR+16,&A_gz_value,2);	//�洢����ǿ����ֵ
			STMFLASH_Write(FLASH_SAVE_ADDR+18,&A_m2_value,2);	//�洢������ֵ
			STMFLASH_Write(FLASH_SAVE_ADDR+20,&A_m7_value,2);	//�洢һ����̼��ֵ
			STMFLASH_Write(FLASH_SAVE_ADDR+22,&A_m135_value,2);	//�洢����������ֵ	
	}
	STMFLASH_Read(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//������ȡ�¶���ֵ
	STMFLASH_Read(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//������ȡʪ����ֵ
	STMFLASH_Read(FLASH_SAVE_ADDR+14,&A_pre,2);			//������ȡ��ѹ��ֵ
	STMFLASH_Read(FLASH_SAVE_ADDR+16,&A_gz_value,2);	//������ȡ����ǿ����ֵ
	STMFLASH_Read(FLASH_SAVE_ADDR+18,&A_m2_value,2);	//������ȡ������ֵ
	STMFLASH_Read(FLASH_SAVE_ADDR+20,&A_m7_value,2);	//������ȡһ����̼��ֵ
	STMFLASH_Read(FLASH_SAVE_ADDR+22,&A_m135_value,2);	//������ȡ����������ֵ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	BEEP_Init();//��������ʼ��
	KEY_Init(); //������ʼ��
	DHT11_Init();//��ʼ��DHT11
	LED_Init();		//LED��ʼ����
	IIC_Init();				//I2C���߳�ʼ��
	OLED_Init();			//��ʾ����ʼ��
	OLED_Clear();				//OLED����
	bmp280Init();       //BMP280��ѹ��������ʼ��
	uart1_init(9600);
	Adc1_Channe_Init();//ADC�ӿڳ�ʼ��
	Motor_Init();//���������ʼ��
	Gizwits_Init();//������Э���ʼ��	
	while(1)
	{
		Get_Data(0);
		if(gz_value<=A_gz_value&&mode==0)
		{
			LED1=0;
			curtain_flag=1;
		}else if(gz_value>A_gz_value&&mode==0){
			LED1=1;
			curtain_flag=0;
		}
		if(last_curtain_flag!=curtain_flag&&mode==0)
		{
			BUJING_Cotrol(curtain_flag,3,270);
			last_curtain_flag = curtain_flag;
		}
//		if(last_curtain_flag!=currentDataPoint.valuemotor&&mode==1)
//		{
//			BUJING_Cotrol(currentDataPoint.valuemotor,3,270);
//			curtain_flag = currentDataPoint.valuemotor;
//			last_curtain_flag = currentDataPoint.valuemotor;
//		}	
		if(mode==1)
		{
			LED1=!currentDataPoint.valueled;
			BEEP=!BEEP;
		}
		if(DHT11_Temp>=A_DHT11_Temp||DHT11_Hum<=A_DHT11_Hum||m135_value<=A_m135_value||m2_value>=A_m2_value||m7_value>=A_m7_value
			)
		{
			BEEP = 1;
		}else BEEP = 0;

		key_value = KEY_Scan(0);
		if(key_value==2)
		{
			display_contrl++;
		}
		if(display_contrl%2==0&&t>=10)
		{
			ctrl_comm=1;
			if(ctrl_comm_last != ctrl_comm)//ָ����仯
			{
				OLED_Clear();				//OLED����
				ctrl_comm_last=ctrl_comm;
//				OLED_ShowCHinese(16,0,62);//���ڻ������ϵͳ
//				OLED_ShowCHinese(32,0,63);
//				OLED_ShowCHinese(48,0,64);
//				OLED_ShowCHinese(64,0,65);
//				OLED_ShowCHinese(80,0,66);
//				OLED_ShowCHinese(96,0,67);
				

				OLED_ShowCHinese(0,0,62);
				OLED_ShowCHinese(16,0,63);//���ڻ������ϵͳ
				OLED_ShowCHinese(32,0,64);
				OLED_ShowCHinese(48,0,65);
				OLED_ShowCHinese(64,0,66);
				OLED_ShowCHinese(80,0,67);
				OLED_ShowCHinese(96,0,68);
				OLED_ShowCHinese(112,0,69);
			}
			OLED_ShowCHinese(0,2,0);
			OLED_ShowCHinese(16,2,2);
			sprintf((char*)buff,":%2dC",DHT11_Temp);
			OLED_ShowString(32,2,buff,16);	//��ʾ�¶�
			OLED_ShowCHinese(64,2,1);
			OLED_ShowCHinese(80,2,2);
			sprintf((char*)buff,":%2d%%",DHT11_Hum);
			OLED_ShowString(96,2,buff,16);//��ʾʪ��	
			
			OLED_ShowCHinese(0,4,3);
			OLED_ShowCHinese(16,4,4);
			sprintf((char*)buff,":%4dhpa",Pre);
			OLED_ShowString(32,4,buff,16);//��ʾ��ѹ
			
			OLED_ShowCHinese(0,6,5);
			OLED_ShowCHinese(16,6,6);
			OLED_ShowCHinese(32,6,7);
			OLED_ShowCHinese(48,6,8);
			sprintf((char*)buff,":%4dppm",gz_value);
			OLED_ShowString(64,6,buff,16);//��ʾ����ǿ��
		}else if(display_contrl%2==1&&t>=10)
		{
			ctrl_comm=2;
			if(ctrl_comm_last != ctrl_comm)//ָ����仯
			{
				OLED_Clear();				//OLED����
				ctrl_comm_last=ctrl_comm;
				OLED_ShowCHinese(16,0,62);//���ܼҾ�ϵͳ
				OLED_ShowCHinese(32,0,63);
				OLED_ShowCHinese(48,0,64);
				OLED_ShowCHinese(64,0,65);
				OLED_ShowCHinese(80,0,66);
				OLED_ShowCHinese(96,0,67);
			}
			OLED_ShowCHinese(0,2,13);
			OLED_ShowCHinese(16,2,14);
			OLED_ShowCHinese(32,2,15);
			OLED_ShowCHinese(48,2,16);
			sprintf((char*)buff,":%4dppm",m2_value);
			OLED_ShowString(64,2,buff,16);//��ʾM2����
			
			OLED_ShowCHinese(0,4,17);
			OLED_ShowCHinese(16,4,18);
			OLED_ShowCHinese(32,4,19);
			OLED_ShowCHinese(48,4,20);
			sprintf((char*)buff,":%4dppm",m7_value);
			OLED_ShowString(64,4,buff,16);//��ʾM7����
			
			OLED_ShowCHinese(0,6,9);
			OLED_ShowCHinese(16,6,10);
			OLED_ShowCHinese(32,6,11);
			OLED_ShowCHinese(48,6,12);
			sprintf((char*)buff,":%4dppm",m135_value);
			OLED_ShowString(64,6,buff,16);//��ʾM135����
		}

		userHandle();		//�û����ݲɼ�
		WIFI_Contection(key_value);//WiFi���ӿ���
		gizwitsHandle((dataPoint_t *)&currentDataPoint);//������Э�鴦��
		Canshu_Change(key_value);
		t++;
		delay_ms(100);
	}
}
void Canshu_Change(u8 key)
{
	u8 obj=7;
	if(key==1)
	{
		BEEP=0;
		OLED_Clear();
		while(1)
		{
			key = KEY_Scan(0);
			if(key==1)
			{
				obj++;
				if(obj>=8)
				{
					obj=0;
				}
			}
			sprintf((char *)buff,"Working md:%4d",mode);
			OLED_ShowString(8,0,buff,12);
			sprintf((char *)buff,"A_Temp    :%4d",A_DHT11_Temp);
			OLED_ShowString(8,1,buff,12);
			sprintf((char *)buff,"A_Hum     :%4d",A_DHT11_Hum);
			OLED_ShowString(8,2,buff,12);
			sprintf((char *)buff,"A_pre     :%4d",A_pre);
			OLED_ShowString(8,3,buff,12);
			sprintf((char *)buff,"A_gz_val  :%4d",A_gz_value);
			OLED_ShowString(8,4,buff,12);
			sprintf((char *)buff,"A_m2_val  :%4d",A_m2_value);
			OLED_ShowString(8,5,buff,12);
			sprintf((char *)buff,"A_m7_val  :%4d",A_m7_value);
			OLED_ShowString(8,6,buff,12);
			sprintf((char *)buff,"A_m135_val:%4d",A_m135_value);
			OLED_ShowString(8,7,buff,12);
			if(obj==0)
			{
				OLED_ShowString(0,0," ",12);
				OLED_ShowString(0,1,">",12);
				OLED_ShowString(0,2," ",12);
				OLED_ShowString(0,3," ",12);
				OLED_ShowString(0,4," ",12);
				OLED_ShowString(0,5," ",12);
				OLED_ShowString(0,6," ",12);
				OLED_ShowString(0,7," ",12);
			}
			if(obj==1)
			{
				OLED_ShowString(0,0," ",12);
				OLED_ShowString(0,1," ",12);
				OLED_ShowString(0,2,">",12);
				OLED_ShowString(0,3," ",12);
				OLED_ShowString(0,4," ",12);
				OLED_ShowString(0,5," ",12);
				OLED_ShowString(0,6," ",12);
				OLED_ShowString(0,7," ",12);
			}
			if(obj==2)
			{
				OLED_ShowString(0,0," ",12);
				OLED_ShowString(0,1," ",12);
				OLED_ShowString(0,2," ",12);
				OLED_ShowString(0,3,">",12);
				OLED_ShowString(0,4," ",12);
				OLED_ShowString(0,5," ",12);
				OLED_ShowString(0,6," ",12);
				OLED_ShowString(0,7," ",12);
			}
			if(obj==3)
			{
				OLED_ShowString(0,0," ",12);
				OLED_ShowString(0,1," ",12);
				OLED_ShowString(0,2," ",12);
				OLED_ShowString(0,3," ",12);
				OLED_ShowString(0,4,">",12);
				OLED_ShowString(0,5," ",12);
				OLED_ShowString(0,6," ",12);
				OLED_ShowString(0,7," ",12);
			}
			if(obj==4)
			{
				OLED_ShowString(0,0," ",12);
				OLED_ShowString(0,1," ",12);
				OLED_ShowString(0,2," ",12);
				OLED_ShowString(0,3," ",12);
				OLED_ShowString(0,4," ",12);
				OLED_ShowString(0,5,">",12);
				OLED_ShowString(0,6," ",12);
				OLED_ShowString(0,7," ",12);
			}
			if(obj==5)
			{
				OLED_ShowString(0,0," ",12);
				OLED_ShowString(0,1," ",12);
				OLED_ShowString(0,2," ",12);
				OLED_ShowString(0,3," ",12);
				OLED_ShowString(0,4," ",12);
				OLED_ShowString(0,5," ",12);
				OLED_ShowString(0,6,">",12);
				OLED_ShowString(0,7," ",12);
			}
			if(obj==6)
			{
				OLED_ShowString(0,0," ",12);
				OLED_ShowString(0,1," ",12);
				OLED_ShowString(0,2," ",12);
				OLED_ShowString(0,3," ",12);
				OLED_ShowString(0,4," ",12);
				OLED_ShowString(0,5," ",12);
				OLED_ShowString(0,6," ",12);
				OLED_ShowString(0,7,">",12);
			}
			if(obj==7)
			{
				OLED_ShowString(0,0,">",12);
				OLED_ShowString(0,1," ",12);
				OLED_ShowString(0,2," ",12);
				OLED_ShowString(0,3," ",12);
				OLED_ShowString(0,4," ",12);
				OLED_ShowString(0,5," ",12);
				OLED_ShowString(0,6," ",12);
				OLED_ShowString(0,7," ",12);
			}
			if(obj==0)
			{
				if(key==3)
				{
					A_DHT11_Temp+=1;
				}
				if(key==4)
				{
					A_DHT11_Temp-=1;
				}
			}
			if(obj==1)
			{
				if(key==3)
				{
					A_DHT11_Hum+=1;
				}
				if(key==4)
				{
					A_DHT11_Hum-=1;
				}
			}
			if(obj==2)
			{
				if(key==3)
				{
					A_pre+=20;
				}
				if(key==4)
				{
					A_pre-=20;
				}
			}
			if(obj==3)
			{
				if(key==3)
				{
					A_gz_value+=50;
				}
				if(key==4)
				{
					A_gz_value-=50;
				}
			}
			if(obj==4)
			{
				if(key==3)
				{
					A_m2_value+=50;
				}
				if(key==4)
				{
					A_m2_value-=50;
				}
			}
			if(obj==5)
			{
				if(key==3)
				{
					A_m7_value+=50;
				}
				if(key==4)
				{
					A_m7_value-=50;
				}
			}
			if(obj==6)
			{
				if(key==3)
				{
					A_m135_value+=50;
				}
				if(key==4)
				{
					A_m135_value-=50;
				}
			}
			if(obj==7)
			{
				if(key==3)
				{
					mode+=1;
				}
				if(key==4)
				{
					mode-=1;
				}
				if(mode >= 2) mode = 0;
			}
			if(key==2)
			{
				OLED_Clear();
				OLED_ShowCHinese(16,0,62);//���ܼҾ�ϵͳ
				OLED_ShowCHinese(32,0,63);
				OLED_ShowCHinese(48,0,64);
				OLED_ShowCHinese(64,0,65);
				OLED_ShowCHinese(80,0,66);
				OLED_ShowCHinese(96,0,67);
				STMFLASH_Write(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//�洢�¶���ֵ
				STMFLASH_Write(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//�洢ʪ����ֵ
				STMFLASH_Write(FLASH_SAVE_ADDR+14,&A_pre,2);			//�洢��ѹ��ֵ
				STMFLASH_Write(FLASH_SAVE_ADDR+16,&A_gz_value,2);	//�洢����ǿ����ֵ
				STMFLASH_Write(FLASH_SAVE_ADDR+18,&A_m2_value,2);	//�洢������ֵ
				STMFLASH_Write(FLASH_SAVE_ADDR+20,&A_m7_value,2);	//�洢һ����̼��ֵ
				STMFLASH_Write(FLASH_SAVE_ADDR+22,&A_m135_value,2);	//�洢����������ֵ
				break;
			}
		
		}
	}
}
//Э���ʼ��
void Gizwits_Init(void)
{	
	TIM3_Int_Init(9,7199);//1MSϵͳ��ʱ
  usart2_init(9600);//WIFI��ʼ��
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//�豸״̬�ṹ���ʼ��
	gizwitsInit();//��������ʼ��
}
void Get_Data(u16 count)//��ȡ����������
{
	static float bmp280_press,bmp280_temp,bmp280_asi;				//��ѹ
	bmp280GetData(&bmp280_press,&bmp280_temp,&bmp280_asi);
	Pre = bmp280_press;
	DHT11_Read_Data(&DHT11_Temp,&DHT11_Hum);

	gz_value   = 4096 - get_Adc_Value(0x01);
	
	m2_value   = get_Adc_Value(0x05);
	
	m7_value   = get_Adc_Value(0x06);
	
	m135_value = 4096 - get_Adc_Value(0x07);
}
void WIFI_Contection(u8 key)//WiFi���ӿ���
{
	if(key==3)
	{
		gizwitsSetMode(WIFI_SOFTAP_MODE);//soft-apģʽ����
	}			
	if(key==4)
	{  
		gizwitsSetMode(WIFI_RESET_MODE);//WIFI��λ
	}
}
/*********************************************END OF FILE**********************/
