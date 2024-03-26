/****************************---STM32驱动TDC-GP22代码参考---*****************************/

/************************--TDC-GP22  SPI1模式 最高时钟频率20MHz-----***********************/

/*********************************--相关IO口配置参阅对应.h文件--****************************/

/*******--调试步骤：SPI通信成功->检查激励波形->中断能否正常触发—>读出结果—>滤波拟合---*******/

/*****--每个项目管道，流体，探头皆有差异，拟合可根据matlab采用时间体积法或其他方法拟定--****/
#include "sys.h"
#include "spi.h"
#include "delay.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"
#include "led.h"
#include "stm32f4xx_conf.h"


/********************SPI接口定义-开头****************************/
//CLK  PA5
//MISO PA6
//MOSI PA7
//SSN  PA4
void SPI_TDC_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* 使能 FLASH_SPI 及GPIO 时钟 */
  
	RCC_AHB1PeriphClockCmd (TDC_SPI_SCK_GPIO_CLK | 
	                        TDC_SPI_MISO_GPIO_CLK|
	                        TDC_SPI_MOSI_GPIO_CLK|
	                        TDC_CS_GPIO_CLK, ENABLE);

	/*!< SPI_FLASH_SPI 时钟使能 */
	TDC_SPI_CLK_INIT(TDC_SPI_CLK, ENABLE);
 
	//设置引脚复用
	GPIO_PinAFConfig(TDC_SPI_SCK_GPIO_PORT,TDC_SPI_SCK_PINSOURCE,TDC_SPI_SCK_AF); 
	GPIO_PinAFConfig(TDC_SPI_MISO_GPIO_PORT,TDC_SPI_MISO_PINSOURCE,TDC_SPI_MISO_AF); 
	GPIO_PinAFConfig(TDC_SPI_MOSI_GPIO_PORT,TDC_SPI_MOSI_PINSOURCE,TDC_SPI_MOSI_AF); 
  
	/*!< 配置 SPI_TDC_SPI 引脚: SCK */
	GPIO_InitStructure.GPIO_Pin = TDC_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  
	GPIO_Init(TDC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< 配置 SPI_TDC_SPI 引脚: MISO */
	GPIO_InitStructure.GPIO_Pin = TDC_SPI_MISO_PIN;
	GPIO_Init(TDC_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< 配置 SPI_TDC_SPI 引脚: MOSI */
	GPIO_InitStructure.GPIO_Pin = TDC_SPI_MOSI_PIN;
	GPIO_Init(TDC_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< 配置 SPI_TDC_SPI 引脚: CS */
	GPIO_InitStructure.GPIO_Pin = TDC_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(TDC_CS_GPIO_PORT, &GPIO_InitStructure);

	/* 停止信号 TDC: CS引脚高电平*/
	TDC_CS_1;

	/* TDC_SPI 模式配置 */
	// TDC芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(TDC_SPI, &SPI_InitStructure);

	/* 使能 TDC_SPI  */
	SPI_Cmd(TDC_SPI, ENABLE);

}

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
		
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
 		    
}


/***************************************************************************
 * DotHextoDotDec function.
 *
 * @brief	Convert Dot HEX to Dot DEC of the char.
 **************************************************************************/
float dotHextoDotDec(unsigned long dotHex)
{
	float dotDec = 0;
	
	dotDec = (float)((dotHex >> 16) & 0xFFFF) + (float)((dotHex & 0xFFFF) / 65536.0);//这样是否更简洁?
	
	return dotDec*0.25;//为什么要乘以0.25？
}


uint32_t SPI1_ReadWordFromReg(uint8_t regAddr)
{
	uint32_t res = 0;

	TDC_CS_0;
	delay_us(3);
	SPI1_ReadWriteByte(regAddr);
	//读第一个字节
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	res <<= 8;
	//读第二个字节
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	res <<= 8;
	//读第三个字节
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	res <<= 8;
	//读第四个字节
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	delay_us(3);
	TDC_CS_1;//P9.4 TDC_SSN置高 

	return res;
}


uint16_t SPI1_Read2ByteFromReg(uint8_t regAddr)
{
	uint16_t res = 0;

	TDC_CS_0;
	delay_us(3);
	SPI1_ReadWriteByte(regAddr);
	//读第一个字节
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	res <<= 8;
	res |= SPI1_ReadWriteByte(DUMMY_DATA);

	delay_us(3);

	TDC_CS_1;//P9.4 TDC_SSN置高 

	return res;
}


uint8_t SPI1_ReadByteFromReg(uint8_t regAddr)
{
	uint8_t res = 0;

	TDC_CS_0;
	delay_us(3);
	SPI1_ReadWriteByte(regAddr);
	delay_us(3);
	//读第一个字节
	res = SPI1_ReadWriteByte(DUMMY_DATA);
	delay_us(3);
	TDC_CS_1;//P9.4 TDC_SSN置高 

	return res;
}



