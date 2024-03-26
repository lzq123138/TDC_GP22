/****************************---STM32驱动TDC-GP22代码参考---*****************************/

/************************--TDC-GP22  SPI1模式 最高时钟频率20MHz-----***********************/

/*********************************--相关IO口配置参阅对应.h文件--****************************/

/*******--调试步骤：SPI通信成功->检查激励波形->中断能否正常触发—>读出结果—>滤波拟合---*******/

/*****--每个项目管道，流体，探头皆有差异，拟合可根据matlab采用时间体积法或其他方法拟定--****/

#include "stdio.h"
#include "sys.h"
#include "math.h"
#include "delay.h"
#include "usart.h"	
#include "led.h"
#include "lcd.h"
#include "spi.h"
#include "tdc.h"
u16 byte;//变量类型声明
float  bytes_Cal=0.00000f;//校准系数
float  bytes_Up=0.00000f;
float  time_one=0.00000f;
float  time_two=0.00000f;
float  time_three=0.00000f;
float  time_four=0.00000f;
float  temp=0.00000f;

float  bytes_Up_Sum=0.00000f;
float  bytes_Up_Ave=0.00000f;
float  bytes_Down=0.00000f;
float  bytes_Down_Sum=0.00000f;
float  bytes_Down_Ave=0.00000f;
float  t=0.00000f;
float  t_abs=0.00000f;
float  t_Ave=0.00000f;
float  t2_Ave=0.00000f;
float  t_Sum=0.00000f;
float  Q = 0.00000000f;
float  threshold=0.00000f;
u16 a=100;
u16 i=0;
u16 ii=0;
u16 iii=0;
u16 jjj=0;
//float Pw1st_Byte=0.0000f;//脉冲宽度
extern  u16 G_tdcStatusRegister;//状态寄存器
char    bytes_Up_char[8];
char    bytes_Up_Sum_char[8];
char    bytes_Up_Ave_char[8];
char    bytes_Down_char[8];
char    bytes_Down_Sum_char[8];
char    bytes_Down_Ave_char[8];
char    t_char[8];
char    t_Ave_char[8];
char    t2_Ave_char[8];
char    Q_char[8];
char    i_char[16];
char    bytes_V_char[8];
	
int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2	
	delay_init(168);    
	uart_init(115200);	  //串口初始化波特率为38400
	//LED_Init();					//初始化LED
 	//OLED_Init();				//初始化 0.96OLED 
	SPI_TDC_Init();     //SPI通信初始化
	TDC_GPIO_Init();    
	TDC_Init_Reg();     

	EXTIX_Init(); 
	//OLED_Clear();	
	//lcd 初始化那些东西放这里，以及一些字符串不变的显示


	
	printf("上游溢出\t\r\n");// 测量溢出 
	//先接 vcc3.3 GND spi四根线  RSTN  一共7根线 

	
	/********************SPI接口定义-开头****************************/
//CLK  PA5
//MISO PA6
//MOSI PA7
//SSN  PA4   RSTN PA8  接

/*TDC接口定义****************************/

/*
PA12 STA
PA11 DIS
PA10 INT
PA9  FIRE_IN
PA8  RSTN     超声探头红的接sp1 sp2 白线都接地  

*/
	byte=SPI1_ReadByteFromReg(READ_REG1);//显示寄存器1中的高8位	这个值是0x21的时候，才说明通信成功了成功了
	calibrateResonator();//校准（）
	bytes_Cal=244.14f/(dotHextoDotDec(SPI1_ReadWordFromReg(READ_RES0)));
	delay_us(10);

	//开始测量
/********************************************************


********************************************************/
	threshold = 0.02;   //threshold=2*v*L*tan/c/c
	while(1)
	{   
		initMeasureTDCGP22();                    
		timeFlightRestartTDCGP22();	
		while(INTSign == 0){}			
		INTSign = 0;
		G_tdcStatusRegister=SPI1_Read2ByteFromReg(READ_STAT);				
		if((G_tdcStatusRegister&0x0600) != 0) //0000 0110  0000 0000
		{
			printf("上游溢出\t\r\n");// 测量溢出 
		}
		else
		{
			bytes_Up=dotHextoDotDec(SPI1_ReadWordFromReg(READ_RES3))*bytes_Cal/3-9;//bytes_Up 这是一个值
			printf("bytes_Up(us)= %f \t\r\n\n",bytes_Up);
		}
		initMeasureTDCGP22();                     
		while(INTSign == 0){}				  
			INTSign = 0;
		G_tdcStatusRegister=SPI1_Read2ByteFromReg(READ_STAT);
		if((G_tdcStatusRegister&0x0600) != 0)
		{
			printf("下游溢出\t\r\n")        ;// 测量溢出
		}
		else
		{
			bytes_Down=dotHextoDotDec(SPI1_ReadWordFromReg(READ_RES3))*bytes_Cal/3-9;//bytes_Down 这是一个值
			printf("bytes_Down(us)= %f \t\r\n\n",bytes_Down);
		}
	}	
}

