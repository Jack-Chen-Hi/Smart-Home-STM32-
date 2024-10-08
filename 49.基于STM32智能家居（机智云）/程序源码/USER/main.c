#include "sys.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "ADC.h"
#include "usart3.h"
#include "key.h"
#include "oled.h"
#include "math.h"
#include "dht11.h"
#include "bmp280.h"
#include "gizwits_product.h" 

u32 STM32_xx0=0X4E4C4A;
u32 STM32_xx1=0X364B1322;
u32 STM32_xx2=0X132D13  ;

#define FLASH_SAVE_ADDR 0x08010000

u8 buff[30];//������ʾ��������
u8 count;
dataPoint_t currentDataPoint;//�ƶ����ݵ�
u8 wifi_sta;//wifi����״̬��־
u8 mode=0;//ģʽ�����Զ����ֶ�
u8 NTP_sta=0;//��ŵʱ���ȡ״̬
u8 display_contrl=0;
u8 curtain_flag=0;
u8 last_curtain_flag=0;

extern u8 DHT11_Temp,DHT11_Hum;		//��ʪ��
u16 Pre;							//��ѹ���ֵ
u16 gz_value;						//���ռ��ֵ			
u16 m2_value;						//������ֵ
u16 m7_value;						//һ����̼���ֵ
u16 m135_value;						//�����������ֵ

u16 A_DHT11_Temp=35;				//�¶���ֵ   			 ������ֵʱ����
u16 A_DHT11_Hum=20;					//ʪ����ֵ				 ������ֵʱ����
u16 A_pre=1500;							//��ѹ��ֵ				 ������ֵʱ����
u16 A_gz_value=1000;				//����ǿ����ֵ		 ������ֵʱ�򿪴�����������
u16 A_m2_value=4000;				//������ֵ				 ������ֵʱ����
u16 A_m7_value=4000;				//һ����̼��ֵ  	 ������ֵʱ����
u16 A_m135_value=1500;			//����������ֵ		 ������ֵʱ����
/************************************************
 ALIENTEK��ӢSTM32������ʵ��1
 �����ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
//Э���ʼ��
void Gizwits_Init(void)
{	
	TIM3_Int_Init(9,7199);//1MSϵͳ��ʱ
    usart3_init(9600);//WIFI��ʼ��
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//�豸״̬�ṹ���ʼ��
	gizwitsInit();//��������ʼ��
}

//���ݲɼ�
void userHandle(void)
{
	currentDataPoint.valueLED=!LED0;
	currentDataPoint.valueCurtain=curtain_flag;
	currentDataPoint.valueTemp=DHT11_Temp;
	currentDataPoint.valueHum=DHT11_Hum;
	currentDataPoint.valuePre=Pre;
	currentDataPoint.valueGZ_Value=gz_value;
	currentDataPoint.valueMQ2_Value=m2_value;
	currentDataPoint.valueMQ7_Value=m7_value;
	currentDataPoint.valueMQ135_Value=m135_value;
}

void Get_Data(u16 count);//��ȡ����������
void WIFI_Contection(u8 key);//WiFi���ӿ���
void Canshu_Change(u8 key);//ϵͳ��������
void Mode_Change(u8 key);//ģʽ�л�
void BUJING_Cotrol(u8 mode,u16 time,u16 count);

int main(void)
{	
	u8 t=0;
	int key_value;		//����ֵ
	uart_init(115200);	 		//���ڳ�ʼ��Ϊ115200
	delay_init();	    //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	Adc1_Channe_Init();			//ADCͨ����ʼ��
	KEY_Init();					//�������ų�ʼ��
	bmp280Init();
	//while(DHT11_Init());	
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	OLED_Init();				//OLED��ʼ��
	OLED_Clear();				//OLED����
	Gizwits_Init();		
	
//	STMFLASH_Write(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//������ȡ�¶���ֵ
//	STMFLASH_Write(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//������ȡʪ����ֵ
//	STMFLASH_Write(FLASH_SAVE_ADDR+14,&A_pre,2);			//������ȡ��ѹ��ֵ
//	STMFLASH_Write(FLASH_SAVE_ADDR+18,&A_gz_value,2);	//������ȡ����ǿ����ֵ
//	STMFLASH_Write(FLASH_SAVE_ADDR+20,&A_m2_value,2);	//������ȡ������ֵ
//	STMFLASH_Write(FLASH_SAVE_ADDR+22,&A_m7_value,2);	//������ȡһ����̼��ֵ
//	STMFLASH_Write(FLASH_SAVE_ADDR+24,&A_m135_value,2);	//������ȡ����������ֵ
	
//	STMFLASH_Read(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//������ȡ�¶���ֵ
//	STMFLASH_Read(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//������ȡʪ����ֵ
//	STMFLASH_Read(FLASH_SAVE_ADDR+14,&A_pre,2);			//������ȡ��ѹ��ֵ
//	STMFLASH_Read(FLASH_SAVE_ADDR+18,&A_gz_value,2);	//������ȡ����ǿ����ֵ
//	STMFLASH_Read(FLASH_SAVE_ADDR+20,&A_m2_value,2);	//������ȡ������ֵ
//	STMFLASH_Read(FLASH_SAVE_ADDR+22,&A_m7_value,2);	//������ȡһ����̼��ֵ
//	STMFLASH_Read(FLASH_SAVE_ADDR+24,&A_m135_value,2);	//������ȡ����������ֵ

	while(1)
	{
		Get_Data(0);
		if(gz_value<=A_gz_value&&mode==0)
		{
			LED0=0;
			curtain_flag=0;
		}else if(gz_value>A_gz_value&&mode==0){
			LED0=1;
			curtain_flag=1;
		}
		if(last_curtain_flag!=curtain_flag&&mode==0)
		{
			BUJING_Cotrol(curtain_flag,3,270);
			last_curtain_flag = curtain_flag;
		}
		if(last_curtain_flag!=currentDataPoint.valueCurtain&&mode==1)
		{
			BUJING_Cotrol(currentDataPoint.valueCurtain,3,270);
			curtain_flag = currentDataPoint.valueCurtain;
			last_curtain_flag = currentDataPoint.valueCurtain;
		}	
		if(mode==1)
		{
			LED0=!currentDataPoint.valueLED;
		}
		if(DHT11_Temp>=A_DHT11_Temp||DHT11_Hum<=A_DHT11_Hum||Pre>=A_pre||m135_value<=A_m135_value||m2_value>=A_m2_value||m7_value>=A_m7_value)
		{
			BEEP=!BEEP;
		}else BEEP = 0;

		key_value = KEY_Scan(0);
		if(key_value==2)
		{
			display_contrl++;
		}
		if(display_contrl%2==0&&t>=10)
		{
			OLED_ShowCHinese(0,0,0);
			OLED_ShowCHinese(16,0,2);
			sprintf((char*)buff,":%2dC",DHT11_Temp);
			OLED_ShowString(32,0,buff,16);	//��ʾ�¶�
			OLED_ShowCHinese(64,0,1);
			OLED_ShowCHinese(82,0,2);
			sprintf((char*)buff,":%2d%%",DHT11_Hum);
			OLED_ShowString(96,0,buff,16);//��ʾʪ��	
			
			OLED_ShowCHinese(0,2,3);
			OLED_ShowCHinese(16,2,4);
			sprintf((char*)buff,":%2dhpa     ",Pre);
			OLED_ShowString(32,2,buff,16);//��ʾ��ѹ
			
			OLED_ShowCHinese(0,4,5);
			OLED_ShowCHinese(16,4,6);
			OLED_ShowCHinese(32,4,7);
			OLED_ShowCHinese(48,4,8);
			sprintf((char*)buff,":%4dppm",gz_value);
			OLED_ShowString(64,4,buff,16);//��ʾ����ǿ��
			
			OLED_ShowCHinese(0,6,9);
			OLED_ShowCHinese(16,6,10);
			OLED_ShowCHinese(32,6,11);
			OLED_ShowCHinese(48,6,12);
			sprintf((char*)buff,":%4dppm",m135_value);
			OLED_ShowString(64,6,buff,16);//��ʾ��������
		}else if(display_contrl%2==1&&t>=10)
		{
			OLED_ShowCHinese(0,0,13);
			OLED_ShowCHinese(16,0,14);
			OLED_ShowCHinese(32,0,15);
			OLED_ShowCHinese(48,0,16);
			sprintf((char*)buff,":%4dppm",m2_value);
			OLED_ShowString(64,0,buff,16);//��ʾM2����
			
			OLED_ShowCHinese(0,2,17);
			OLED_ShowCHinese(16,2,18);
			OLED_ShowCHinese(32,2,19);
			OLED_ShowCHinese(48,2,20);
			sprintf((char*)buff,":%4dppm",m7_value);
			OLED_ShowString(64,2,buff,16);//��ʾM7����
			
			OLED_ShowCHinese(0,4,5);
			OLED_ShowCHinese(16,4,6);
			OLED_ShowCHinese(32,4,7);
			OLED_ShowCHinese(48,4,8);
			sprintf((char*)buff,":%4dppm",gz_value);
			OLED_ShowString(64,4,buff,16);//��ʾ����ǿ��
			
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
 
void WIFI_Contection(u8 key)//WiFi���ӿ���
{
	if(key==2)
	{
		printf("WIFI����AirLink����ģʽ\r\n");
		gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-linkģʽ����
	}			
	if(key==3)
	{  
		
		printf("WIFI��λ����������������\r\n");
		gizwitsSetMode(WIFI_RESET_MODE);//WIFI��λ
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
//				STMFLASH_Write(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//������ȡ�¶���ֵ
//				STMFLASH_Write(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//������ȡʪ����ֵ
//				STMFLASH_Write(FLASH_SAVE_ADDR+14,&A_pre,2);			//������ȡ��ѹ��ֵ
//				STMFLASH_Write(FLASH_SAVE_ADDR+18,&A_gz_value,2);	//������ȡ����ǿ����ֵ
//				STMFLASH_Write(FLASH_SAVE_ADDR+20,&A_m2_value,2);	//������ȡ������ֵ
//				STMFLASH_Write(FLASH_SAVE_ADDR+22,&A_m7_value,2);	//������ȡһ����̼��ֵ
//				STMFLASH_Write(FLASH_SAVE_ADDR+24,&A_m135_value,2);	//������ȡ����������ֵ
				OLED_Clear();
				break;
			}
		
		}
	}
}

void Get_Data(u16 count)//��ȡ����������
{
	static float bmp280_press,bmp280;				//��ѹ
	
	DHT11_Read_Data(&DHT11_Temp,&DHT11_Hum);
	
	bmp280GetData(&bmp280_press,&bmp280,&bmp280);
	Pre = bmp280_press;
	
	gz_value   = 4096 - get_Adc_Value(0x04);
	
	m2_value   = get_Adc_Value(0x07);
	
	m7_value   = get_Adc_Value(0x05);
	
	m135_value = 4096 - get_Adc_Value(0x06);
}
//void Mode_Change(u8 key)
//{
//	if(key==2)
//	{
//		mode++;
//		if(mode>=2) mode = 0;
//	}
//	if(mode==0) OLED_ShowString(0,0,"MODE:Auto",12); //HM
//	if(mode==1) OLED_ShowString(0,0,"MODE:H_M_",12); //HM
//}

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
