/****************************---STM32����TDC-GP22����ο�---*****************************/

/************************--TDC-GP22  SPI1ģʽ ���ʱ��Ƶ��20MHz-----***********************/

/*********************************--���IO�����ò��Ķ�Ӧ.h�ļ�--****************************/

/*******--���Բ��裺SPIͨ�ųɹ�->��鼤������->�ж��ܷ�����������>���������>�˲����---*******/

/*****--ÿ����Ŀ�ܵ������壬̽ͷ���в��죬��Ͽɸ���matlab����ʱ������������������ⶨ--****/

#include "stdio.h"
#include "sys.h"
#include "math.h"
#include "delay.h"
#include "usart.h"	
#include "led.h"
#include "lcd.h"
#include "spi.h"
#include "tdc.h"
u16 byte;//������������
float  bytes_Cal=0.00000f;//У׼ϵ��
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
//float Pw1st_Byte=0.0000f;//������
extern  u16 G_tdcStatusRegister;//״̬�Ĵ���
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2	
	delay_init(168);    
	uart_init(115200);	  //���ڳ�ʼ��������Ϊ38400
	//LED_Init();					//��ʼ��LED
 	//OLED_Init();				//��ʼ�� 0.96OLED 
	SPI_TDC_Init();     //SPIͨ�ų�ʼ��
	TDC_GPIO_Init();    
	TDC_Init_Reg();     

	EXTIX_Init(); 
	//OLED_Clear();	
	//lcd ��ʼ����Щ����������Լ�һЩ�ַ����������ʾ


	
	printf("�������\t\r\n");// ������� 
	//�Ƚ� vcc3.3 GND spi�ĸ���  RSTN  һ��7���� 

	
	/********************SPI�ӿڶ���-��ͷ****************************/
//CLK  PA5
//MISO PA6
//MOSI PA7
//SSN  PA4   RSTN PA8  ��

/*TDC�ӿڶ���****************************/

/*
PA12 STA
PA11 DIS
PA10 INT
PA9  FIRE_IN
PA8  RSTN     ����̽ͷ��Ľ�sp1 sp2 ���߶��ӵ�  

*/
	byte=SPI1_ReadByteFromReg(READ_REG1);//��ʾ�Ĵ���1�еĸ�8λ	���ֵ��0x21��ʱ�򣬲�˵��ͨ�ųɹ��˳ɹ���
	calibrateResonator();//У׼����
	bytes_Cal=244.14f/(dotHextoDotDec(SPI1_ReadWordFromReg(READ_RES0)));
	delay_us(10);

	//��ʼ����
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
			printf("�������\t\r\n");// ������� 
		}
		else
		{
			bytes_Up=dotHextoDotDec(SPI1_ReadWordFromReg(READ_RES3))*bytes_Cal/3-9;//bytes_Up ����һ��ֵ
			printf("bytes_Up(us)= %f \t\r\n\n",bytes_Up);
		}
		initMeasureTDCGP22();                     
		while(INTSign == 0){}				  
			INTSign = 0;
		G_tdcStatusRegister=SPI1_Read2ByteFromReg(READ_STAT);
		if((G_tdcStatusRegister&0x0600) != 0)
		{
			printf("�������\t\r\n")        ;// �������
		}
		else
		{
			bytes_Down=dotHextoDotDec(SPI1_ReadWordFromReg(READ_RES3))*bytes_Cal/3-9;//bytes_Down ����һ��ֵ
			printf("bytes_Down(us)= %f \t\r\n\n",bytes_Down);
		}
	}	
}

