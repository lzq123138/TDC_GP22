/****************************---STM32驱动TDC-GP22代码参考---*****************************/

/************************--TDC-GP22  SPI1模式 最高时钟频率20MHz-----***********************/

/*********************************--相关IO口配置参阅对应.h文件--****************************/

/*******--调试步骤：SPI通信成功->检查激励波形->中断能否正常触发—>读出结果—>滤波拟合---*******/

/*****--每个项目管道，流体，探头皆有差异，拟合可根据matlab采用时间体积法或其他方法拟定--****/
#include "led.h" 
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
//本段是用来点亮开发板上面一个led灯；如果不需要可以不处理
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟

	//GPIOB0,B8初始化设置
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	
	GPIO_ResetBits(GPIOF,GPIO_Pin_10);//GPIOB0,B8设置高灯亮
	GPIO_SetBits(GPIOF,GPIO_Pin_9);//GPIOB0,B8设置高灯亮
	
}






