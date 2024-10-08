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
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
//***************************设置局部变量***************************//
#define FLASH_SAVE_ADDR  0X0800E000	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
u8 buff[30];//参数显示缓存数组
u8 count;
u8 wifi_sta;//wifi连接状态标志
u8 mode=0;//模式控制自动和手动
u8 NTP_sta=0;//网诺时间获取状态
u8 display_contrl=0;
u8 curtain_flag=0;
u8 last_curtain_flag=0;
char ctrl_comm = 1;//控制指令
char ctrl_comm_last = 0;//上一次的指令
extern u8 DHT11_Temp,DHT11_Hum;		//温湿度
u16 Pre;							//气压检测值
u16 gz_value;						//光照检测值			
u16 m2_value;						//烟雾检测值
u16 m7_value;						//一氧化碳检测值
u16 m135_value;						//空气质量检测值
u16 A_flag;				     //标志信息缓存
u16 A_DHT11_Temp;				//温度阈值缓存
u16 A_DHT11_Hum;					//湿度阈值缓存
u16 A_pre;						//气压阈值缓存
u16 A_gz_value;				//光照强度阈值缓存
u16 A_m2_value;				//烟雾阈值缓存
u16 A_m7_value;				//一氧化碳阈值缓存
u16 A_m135_value;				//空气质量阈值缓存
//*****************************************************************//
void Get_Data(u16 count);//获取传感器数据
void WIFI_Contection(u8 key);//WiFi连接控制
void Canshu_Change(u8 key);//系统参数调节
void Mode_Change(u8 key);//模式切换
void BUJING_Cotrol(u8 mode,u16 time,u16 count);
void Gizwits_Init(void);
int main(void)
{	
	u8 t=0;
	int key_value;		//按键值
	delay_init();	    //延时函数初始化
	STMFLASH_Read(FLASH_SAVE_ADDR+8,&A_flag,2);	//开机读取标志位
	if(A_flag == 65535)
	{
			A_flag=5555;
			A_DHT11_Temp=30;				//温度阈值>30
			A_DHT11_Hum=35;					//湿度阈值<35
			A_pre=1500;						  //气压阈值1
			A_gz_value=1500;				//光照强度阈值
			A_m2_value=1000;				//烟雾阈值>
			A_m7_value=1000;				//一氧化碳阈值>
			A_m135_value=2800;				//空气质量阈值<
			STMFLASH_Write(FLASH_SAVE_ADDR+8,&A_flag,2);
			STMFLASH_Write(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//存储温度阈值
			STMFLASH_Write(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//存储湿度阈值
			STMFLASH_Write(FLASH_SAVE_ADDR+14,&A_pre,2);			//存储气压阈值
			STMFLASH_Write(FLASH_SAVE_ADDR+16,&A_gz_value,2);	//存储光照强度阈值
			STMFLASH_Write(FLASH_SAVE_ADDR+18,&A_m2_value,2);	//存储烟雾阈值
			STMFLASH_Write(FLASH_SAVE_ADDR+20,&A_m7_value,2);	//存储一氧化碳阈值
			STMFLASH_Write(FLASH_SAVE_ADDR+22,&A_m135_value,2);	//存储空气质量阈值	
	}
	STMFLASH_Read(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//开机读取温度阈值
	STMFLASH_Read(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//开机读取湿度阈值
	STMFLASH_Read(FLASH_SAVE_ADDR+14,&A_pre,2);			//开机读取气压阈值
	STMFLASH_Read(FLASH_SAVE_ADDR+16,&A_gz_value,2);	//开机读取光照强度阈值
	STMFLASH_Read(FLASH_SAVE_ADDR+18,&A_m2_value,2);	//开机读取烟雾阈值
	STMFLASH_Read(FLASH_SAVE_ADDR+20,&A_m7_value,2);	//开机读取一氧化碳阈值
	STMFLASH_Read(FLASH_SAVE_ADDR+22,&A_m135_value,2);	//开机读取空气质量阈值
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	BEEP_Init();//蜂鸣器初始化
	KEY_Init(); //按键初始化
	DHT11_Init();//初始化DHT11
	LED_Init();		//LED初始化化
	IIC_Init();				//I2C总线初始化
	OLED_Init();			//显示屏初始化
	OLED_Clear();				//OLED清屏
	bmp280Init();       //BMP280气压传感器初始化
	uart1_init(9600);
	Adc1_Channe_Init();//ADC接口初始化
	Motor_Init();//电机驱动初始化
	Gizwits_Init();//机智云协议初始化	
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
			if(ctrl_comm_last != ctrl_comm)//指令发生变化
			{
				OLED_Clear();				//OLED清屏
				ctrl_comm_last=ctrl_comm;
//				OLED_ShowCHinese(16,0,62);//室内环境检测系统
//				OLED_ShowCHinese(32,0,63);
//				OLED_ShowCHinese(48,0,64);
//				OLED_ShowCHinese(64,0,65);
//				OLED_ShowCHinese(80,0,66);
//				OLED_ShowCHinese(96,0,67);
				

				OLED_ShowCHinese(0,0,62);
				OLED_ShowCHinese(16,0,63);//室内环境检测系统
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
			OLED_ShowString(32,2,buff,16);	//显示温度
			OLED_ShowCHinese(64,2,1);
			OLED_ShowCHinese(80,2,2);
			sprintf((char*)buff,":%2d%%",DHT11_Hum);
			OLED_ShowString(96,2,buff,16);//显示湿度	
			
			OLED_ShowCHinese(0,4,3);
			OLED_ShowCHinese(16,4,4);
			sprintf((char*)buff,":%4dhpa",Pre);
			OLED_ShowString(32,4,buff,16);//显示气压
			
			OLED_ShowCHinese(0,6,5);
			OLED_ShowCHinese(16,6,6);
			OLED_ShowCHinese(32,6,7);
			OLED_ShowCHinese(48,6,8);
			sprintf((char*)buff,":%4dppm",gz_value);
			OLED_ShowString(64,6,buff,16);//显示光照强度
		}else if(display_contrl%2==1&&t>=10)
		{
			ctrl_comm=2;
			if(ctrl_comm_last != ctrl_comm)//指令发生变化
			{
				OLED_Clear();				//OLED清屏
				ctrl_comm_last=ctrl_comm;
				OLED_ShowCHinese(16,0,62);//智能家居系统
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
			OLED_ShowString(64,2,buff,16);//显示M2数据
			
			OLED_ShowCHinese(0,4,17);
			OLED_ShowCHinese(16,4,18);
			OLED_ShowCHinese(32,4,19);
			OLED_ShowCHinese(48,4,20);
			sprintf((char*)buff,":%4dppm",m7_value);
			OLED_ShowString(64,4,buff,16);//显示M7数据
			
			OLED_ShowCHinese(0,6,9);
			OLED_ShowCHinese(16,6,10);
			OLED_ShowCHinese(32,6,11);
			OLED_ShowCHinese(48,6,12);
			sprintf((char*)buff,":%4dppm",m135_value);
			OLED_ShowString(64,6,buff,16);//显示M135数据
		}

		userHandle();		//用户数据采集
		WIFI_Contection(key_value);//WiFi连接控制
		gizwitsHandle((dataPoint_t *)&currentDataPoint);//机智云协议处理
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
				OLED_ShowCHinese(16,0,62);//智能家居系统
				OLED_ShowCHinese(32,0,63);
				OLED_ShowCHinese(48,0,64);
				OLED_ShowCHinese(64,0,65);
				OLED_ShowCHinese(80,0,66);
				OLED_ShowCHinese(96,0,67);
				STMFLASH_Write(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//存储温度阈值
				STMFLASH_Write(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//存储湿度阈值
				STMFLASH_Write(FLASH_SAVE_ADDR+14,&A_pre,2);			//存储气压阈值
				STMFLASH_Write(FLASH_SAVE_ADDR+16,&A_gz_value,2);	//存储光照强度阈值
				STMFLASH_Write(FLASH_SAVE_ADDR+18,&A_m2_value,2);	//存储烟雾阈值
				STMFLASH_Write(FLASH_SAVE_ADDR+20,&A_m7_value,2);	//存储一氧化碳阈值
				STMFLASH_Write(FLASH_SAVE_ADDR+22,&A_m135_value,2);	//存储空气质量阈值
				break;
			}
		
		}
	}
}
//协议初始化
void Gizwits_Init(void)
{	
	TIM3_Int_Init(9,7199);//1MS系统定时
  usart2_init(9600);//WIFI初始化
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//设备状态结构体初始化
	gizwitsInit();//缓冲区初始化
}
void Get_Data(u16 count)//获取传感器数据
{
	static float bmp280_press,bmp280_temp,bmp280_asi;				//气压
	bmp280GetData(&bmp280_press,&bmp280_temp,&bmp280_asi);
	Pre = bmp280_press;
	DHT11_Read_Data(&DHT11_Temp,&DHT11_Hum);

	gz_value   = 4096 - get_Adc_Value(0x01);
	
	m2_value   = get_Adc_Value(0x05);
	
	m7_value   = get_Adc_Value(0x06);
	
	m135_value = 4096 - get_Adc_Value(0x07);
}
void WIFI_Contection(u8 key)//WiFi连接控制
{
	if(key==3)
	{
		gizwitsSetMode(WIFI_SOFTAP_MODE);//soft-ap模式接入
	}			
	if(key==4)
	{  
		gizwitsSetMode(WIFI_RESET_MODE);//WIFI复位
	}
}
/*********************************************END OF FILE**********************/
