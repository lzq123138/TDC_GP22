/****************************---STM32����TDC-GP22����ο�---*****************************/

/************************--TDC-GP22  SPI1ģʽ ���ʱ��Ƶ��20MHz-----***********************/

/*********************************--���IO�����ò��Ķ�Ӧ.h�ļ�--****************************/

/*******--���Բ��裺SPIͨ�ųɹ�->��鼤������->�ж��ܷ�����������>���������>�˲����---*******/

/*****--ÿ����Ŀ�ܵ������壬̽ͷ���в��죬��Ͽɸ���matlab����ʱ������������������ⶨ--****/
#include "led.h" 
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
//������������������������һ��led�ƣ��������Ҫ���Բ�����
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��

	//GPIOB0,B8��ʼ������
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
	
	GPIO_ResetBits(GPIOF,GPIO_Pin_10);//GPIOB0,B8���øߵ���
	GPIO_SetBits(GPIOF,GPIO_Pin_9);//GPIOB0,B8���øߵ���
	
}






