/****************************---STM32驱动TDC-GP22代码参考---*****************************/

/************************--TDC-GP22  SPI1模式 最高时钟频率20MHz-----***********************/

/*********************************--相关IO口配置参阅对应.h文件--****************************/

/*******--调试步骤：SPI通信成功->检查激励波形->中断能否正常触发—>读出结果—>滤波拟合---*******/

/*****--每个项目管道，流体，探头皆有差异，拟合可根据matlab采用时间体积法或其他方法拟定--****/
#include "sys.h"
#include "spi.h"
#include "tdc.h"
#include "delay.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"
#include "led.h"
#include "stm32f4xx_conf.h"


//TDC引脚的初始化

void TDC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|TDC_INT_GPIO_CLK , ENABLE);
	//PA12 START  TDC_START_PIN   TDC_START_GPIO_PORT
	GPIO_InitStructure.GPIO_Pin = TDC_START_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(TDC_START_GPIO_PORT, &GPIO_InitStructure);//初始化GPIO

	//PA11 EN_START  TDC_EN_START_PIN   TDC_START_GPIO_PORT
	GPIO_InitStructure.GPIO_Pin = TDC_EN_START_PIN;
	GPIO_Init(TDC_EN_START_GPIO_PORT, &GPIO_InitStructure);
	
	//PA9 FIRE_IN TDC_FIRE_IN_PIN    TDC_FIRE_IN_GPIO_PORT
	GPIO_InitStructure.GPIO_Pin = TDC_FIRE_IN_PIN;
	GPIO_Init(TDC_FIRE_IN_GPIO_PORT, &GPIO_InitStructure);


	//PA8 RESET TDC_RESET_PIN    TDC_RESET_GPIO_PORT
	GPIO_InitStructure.GPIO_Pin = TDC_RESET_PIN;
	GPIO_Init(TDC_RESET_GPIO_PORT, &GPIO_InitStructure);


	
	//PA10 INTN TDC_INT_PIN    TDC_INT_GPIO_PORT 
	//设为输入  中断标志低电平有效
	GPIO_InitStructure.GPIO_Pin = TDC_INT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输出模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(TDC_INT_GPIO_PORT, &GPIO_InitStructure);
	

	TDC_CS_1;
	TDC_START_1;
	TDC_EN_START_1;
	TDC_RESET_1;
}

//tdc寄存器配置
void TDC_Init_Reg( void )
{
  resetTDCGP22();	
  configureRegisterTDCGP22( WRITE_REG0, 0x4389E800 );      /// Configure TDC register. 0100 0011 1000 1001 1110 1000 0000 0000
  configureRegisterTDCGP22( WRITE_REG1, 0x21444001 );      /// Configure TDC register.
  configureRegisterTDCGP22( WRITE_REG2, 0xA0140002 );      /// Configure TDC register.
  configureRegisterTDCGP22( WRITE_REG3, 0xF8A24803 );      /// Configure TDC register.
  configureRegisterTDCGP22( WRITE_REG4, 0x20004A04 );      /// Configure TDC register.
  configureRegisterTDCGP22( WRITE_REG5, 0x40000005 );      /// Configure TDC register.
  configureRegisterTDCGP22( WRITE_REG6, 0xCEF36106 );      /// Configure TDC register.

}



//外部中断初始化程序
//初始化PE9 低电平有效

uint8_t INTSign = 0 ;
uint8_t  num=0;  //记录中断次数
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource10);//PA10 连接到中断线10
	
	/* 配置EXTI_Line10 */
	EXTI_InitStructure.EXTI_Line = TDC_INT_EXTI_Line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
	EXTI_Init(&EXTI_InitStructure);//配置
 
	NVIC_InitStructure.NVIC_IRQChannel = TDC_INT_IRQn;//外部中断2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
  
}
///TDC 中断函数
void TDC_INT_IRQHandler(void)
{

	if(EXTI_GetFlagStatus(TDC_INT_EXTI_Line) != RESET)
	{
		INTSign =  1 ;
		num++;
		EXTI_ClearFlag(TDC_INT_EXTI_Line);
	}
}

void resetTDCGP22(void)
{
	//RSTN 置高再拉低，延时后再置高
	TDC_RESET_1;
	delay_ms(10);
	TDC_RESET_0;
	delay_ms(200);
	TDC_RESET_1; 
	delay_ms(100); 
}

void powerOnResetTDCGP22(void)
{
	TDC_CS_0;//TDC_SSN置低 
	SPI1_ReadWriteByte(POWER_ON_RESET);
	TDC_CS_1;//TDC_SSN置高 
}

void initMeasureTDCGP22(void)
{
	TDC_CS_0;//TDC_SSN置低 
	SPI1_ReadWriteByte(INIT_MEASURE);
	TDC_CS_1;//TDC_SSN置高
}

void timeFlightStartTDCGP22(void)
{
	TDC_CS_0;//TDC_SSN置低
	SPI1_ReadWriteByte(START_TOF);
	TDC_CS_1;//TDC_SSN置高
}

void timeFlightRestartTDCGP22(void)
{
	TDC_CS_0;//TDC_SSN置低
	//delay_us(1);
	SPI1_ReadWriteByte(START_TOF_RESTART);
	delay_us(1);
	TDC_CS_1;//TDC_SSN置高
}

void tempStartTDCGP22(void)
{
	TDC_CS_0;//TDC_SSN置低
	SPI1_ReadWriteByte(START_TEMP);
	TDC_CS_1;//TDC_SSN置高
}
void tempReStartTDCGP22(void)
{
	TDC_CS_0;//TDC_SSN置低
	SPI1_ReadWriteByte(START_TEMP_RESTART);
	TDC_CS_1;//TDC_SSN置高
}

void configureRegisterTDCGP22(uint8_t opcode_address,uint32_t config_reg_data)
{
	unsigned char data_byte_lo;
	unsigned char data_byte_mid1;
	unsigned char data_byte_mid2;
	unsigned char data_byte_hi;
  
	data_byte_lo = (uint8_t)(config_reg_data & 0xff);
	data_byte_mid1 = (uint8_t)(config_reg_data>>8 & 0xff);
	data_byte_mid2 = (uint8_t)(config_reg_data>>16 & 0xff);
	data_byte_hi = (uint8_t)(config_reg_data>>24 & 0xff);
  
	TDC_CS_0;//P9.4 TDC_SSN置低
	SPI1_ReadWriteByte(opcode_address);
	SPI1_ReadWriteByte(data_byte_hi);
	SPI1_ReadWriteByte(data_byte_mid2);
	SPI1_ReadWriteByte(data_byte_mid1);
	SPI1_ReadWriteByte(data_byte_lo); 
	TDC_CS_1;//P9.4 TDC_SSN置高
}


uint16_t readStatusRegisterTDCGP22(void)
{
	unsigned int result = 0;

	TDC_CS_0;
	delay_us(3);
	SPI1_ReadWriteByte(READ_STAT);
	//读第一个字节
	result |= SPI1_ReadWriteByte(DUMMY_DATA);
	result <<= 8;
	//读第二个字节
	result |= SPI1_ReadWriteByte(DUMMY_DATA);
	delay_us(3);
	TDC_CS_1;//P9.4 TDC_SSN置高 
	delay_us(3);
	return result;
}

float readPW1STRegisterTDCGP22(void)
{
	float result = 0;
	unsigned char resultByte;

	TDC_CS_0;
	delay_us(3);
	SPI1_ReadWriteByte(READ_PW1ST);
	//读第一个字节
	resultByte = SPI1_ReadWriteByte(DUMMY_DATA);
	delay_us(3);
	TDC_CS_1;//P9.4 TDC_SSN置高 
	delay_us(3);
	if( (resultByte & 0x80) != 0x00 )
	{
		result += 1;
	}
	resultByte = resultByte & 0x7F;
	result = result + (float)(resultByte >> 3)/16 + (float)((resultByte << 1) & 0x0F)/256; 

	return result;
}


unsigned char ID_Bytes[7];
//testcomunication();
void readIDbytesTDCGP22(void)
{ 
	TDC_CS_0;
	delay_us(3);
	SPI1_ReadWriteByte(READ_IDBIT);
	//读第一个字节
	ID_Bytes[0] = SPI1_ReadWriteByte(0xff);
	//读第二个字节
	ID_Bytes[1] = SPI1_ReadWriteByte(0xff);
	//读第三个字节
	ID_Bytes[2] = SPI1_ReadWriteByte(0xff);
	//读第四个字节
	ID_Bytes[3] = SPI1_ReadWriteByte(0xff);
	//读第五个字节
	ID_Bytes[4] = SPI1_ReadWriteByte(0xff);
	//读第六个字节
	ID_Bytes[5] = SPI1_ReadWriteByte(0xff); 
	//读第七个字节
	ID_Bytes[6] = SPI1_ReadWriteByte(0xff);
	delay_us(3);
	TDC_CS_1;//P9.4 TDC_SSN置高 
	delay_us(3);
}

uint8_t SPI_TDC_SendByte(uint8_t byte)
{
	 
	uint16_t SPITimeout = 10000;

	/* 等待发送缓冲区为空，TXE事件 */
	while (SPI_I2S_GetFlagStatus(TDC_SPI, SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0)  Error();
	}

	/* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
	SPI_I2S_SendData(TDC_SPI, byte);

	SPITimeout = 10000;

	/* 等待接收缓冲区非空，RXNE事件 */
	while (SPI_I2S_GetFlagStatus(TDC_SPI, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0) Error();
	}

	/* 读取数据寄存器，获取接收缓冲区数据 */
	return SPI_I2S_ReceiveData(TDC_SPI);
}
unsigned char G_tdcInterruptFlag=0;
uint16_t      G_tdcStatusRegister=0;
unsigned long G_tdcTimeOutCount=0;
float         G_calibrateResult=0;
float  G_calibrateCorrectionFactor=0.000f;
void calibrateResonator( void )
{
	unsigned long temp;

	TDC_CS_0;//TDC_SSN置低
	SPI1_ReadWriteByte(START_CAL_OSC);
	TDC_CS_1;//TDC_SSN置高

	while( INTSign == 0);
	INTSign = 0;
	G_tdcStatusRegister = readStatusRegisterTDCGP22();

	temp = readRegisterTDCGP22(READ_RES0+(G_tdcStatusRegister&0x0007)-1);
	G_calibrateResult = dotHextoDotDec(temp);
	// 16*(1/32768)/0.25*1000000=1953.125
	G_calibrateCorrectionFactor = 1953.125/G_calibrateResult;
}





uint32_t readRegisterTDCGP22(unsigned char read_opcode_address)
{
	unsigned long result_read = 0;

	TDC_CS_0;
	SPI1_ReadWriteByte(read_opcode_address);
	//读第一个字节
	result_read |= SPI1_ReadWriteByte(DUMMY_DATA);
	result_read <<= 8;
	//读第二个字节
	result_read |= SPI1_ReadWriteByte(DUMMY_DATA);
	result_read <<= 8;
	//读第三个字节
	result_read |= SPI1_ReadWriteByte(DUMMY_DATA);
	result_read <<= 8;
	//读第四个字节
	result_read |= SPI1_ReadWriteByte(DUMMY_DATA);
	TDC_CS_1;
	delay_us(3);
	return result_read;
}

void Error(void)
{  	uint8_t i;
	
	for( i=0;i<5;i++)
	{
		LED0=~LED0;
		delay_ms(100);
    }
}









		
		
		
		
		
	
