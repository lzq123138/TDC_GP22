/****************************---STM32����TDC-GP22����ο�---*****************************/

/************************--TDC-GP22  SPI1ģʽ ���ʱ��Ƶ��20MHz-----***********************/

/*********************************--���IO�����ò��Ķ�Ӧ.h�ļ�--****************************/

/*******--���Բ��裺SPIͨ�ųɹ�->��鼤������->�ж��ܷ�����������>���������>�˲����---*******/

/*****--ÿ����Ŀ�ܵ������壬̽ͷ���в��죬��Ͽɸ���matlab����ʱ������������������ⶨ--****/
#include "sys.h"
#include "spi.h"
#include "delay.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"
#include "led.h"
#include "stm32f4xx_conf.h"


/********************SPI�ӿڶ���-��ͷ****************************/
//CLK  PA5
//MISO PA6
//MOSI PA7
//SSN  PA4
void SPI_TDC_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* ʹ�� FLASH_SPI ��GPIO ʱ�� */
  
	RCC_AHB1PeriphClockCmd (TDC_SPI_SCK_GPIO_CLK | 
	                        TDC_SPI_MISO_GPIO_CLK|
	                        TDC_SPI_MOSI_GPIO_CLK|
	                        TDC_CS_GPIO_CLK, ENABLE);

	/*!< SPI_FLASH_SPI ʱ��ʹ�� */
	TDC_SPI_CLK_INIT(TDC_SPI_CLK, ENABLE);
 
	//�������Ÿ���
	GPIO_PinAFConfig(TDC_SPI_SCK_GPIO_PORT,TDC_SPI_SCK_PINSOURCE,TDC_SPI_SCK_AF); 
	GPIO_PinAFConfig(TDC_SPI_MISO_GPIO_PORT,TDC_SPI_MISO_PINSOURCE,TDC_SPI_MISO_AF); 
	GPIO_PinAFConfig(TDC_SPI_MOSI_GPIO_PORT,TDC_SPI_MOSI_PINSOURCE,TDC_SPI_MOSI_AF); 
  
	/*!< ���� SPI_TDC_SPI ����: SCK */
	GPIO_InitStructure.GPIO_Pin = TDC_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  
	GPIO_Init(TDC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI_TDC_SPI ����: MISO */
	GPIO_InitStructure.GPIO_Pin = TDC_SPI_MISO_PIN;
	GPIO_Init(TDC_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI_TDC_SPI ����: MOSI */
	GPIO_InitStructure.GPIO_Pin = TDC_SPI_MOSI_PIN;
	GPIO_Init(TDC_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< ���� SPI_TDC_SPI ����: CS */
	GPIO_InitStructure.GPIO_Pin = TDC_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(TDC_CS_GPIO_PORT, &GPIO_InitStructure);

	/* ֹͣ�ź� TDC: CS���Ÿߵ�ƽ*/
	TDC_CS_1;

	/* TDC_SPI ģʽ���� */
	// TDCоƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA
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

	/* ʹ�� TDC_SPI  */
	SPI_Cmd(TDC_SPI, ENABLE);

}

//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
uint8_t SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����
		
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
 		    
}


/***************************************************************************
 * DotHextoDotDec function.
 *
 * @brief	Convert Dot HEX to Dot DEC of the char.
 **************************************************************************/
float dotHextoDotDec(unsigned long dotHex)
{
	float dotDec = 0;
	
	dotDec = (float)((dotHex >> 16) & 0xFFFF) + (float)((dotHex & 0xFFFF) / 65536.0);//�����Ƿ�����?
	
	return dotDec*0.25;//ΪʲôҪ����0.25��
}


uint32_t SPI1_ReadWordFromReg(uint8_t regAddr)
{
	uint32_t res = 0;

	TDC_CS_0;
	delay_us(3);
	SPI1_ReadWriteByte(regAddr);
	//����һ���ֽ�
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	res <<= 8;
	//���ڶ����ֽ�
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	res <<= 8;
	//���������ֽ�
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	res <<= 8;
	//�����ĸ��ֽ�
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	delay_us(3);
	TDC_CS_1;//P9.4 TDC_SSN�ø� 

	return res;
}


uint16_t SPI1_Read2ByteFromReg(uint8_t regAddr)
{
	uint16_t res = 0;

	TDC_CS_0;
	delay_us(3);
	SPI1_ReadWriteByte(regAddr);
	//����һ���ֽ�
	res |= SPI1_ReadWriteByte(DUMMY_DATA);
	res <<= 8;
	res |= SPI1_ReadWriteByte(DUMMY_DATA);

	delay_us(3);

	TDC_CS_1;//P9.4 TDC_SSN�ø� 

	return res;
}


uint8_t SPI1_ReadByteFromReg(uint8_t regAddr)
{
	uint8_t res = 0;

	TDC_CS_0;
	delay_us(3);
	SPI1_ReadWriteByte(regAddr);
	delay_us(3);
	//����һ���ֽ�
	res = SPI1_ReadWriteByte(DUMMY_DATA);
	delay_us(3);
	TDC_CS_1;//P9.4 TDC_SSN�ø� 

	return res;
}



