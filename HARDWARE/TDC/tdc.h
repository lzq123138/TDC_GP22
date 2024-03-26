#ifndef __TDC_H
#define __TDC_H

#include "stm32f4xx.h"
#include <stdio.h>

//指令表宏定义
#define START_TOF                  0x01          /// Start TOF measure
#define START_TEMP                 0x02          /// Start temperature measure
#define START_CAL_OSC              0x03          /// Calibrate oscillator
#define START_CAL_TDC              0x04          /// Calibrate TDC
#define START_TOF_RESTART          0x05          /// Restart TOF measure
#define START_TEMP_RESTART         0x06          /// Restart temperature measure

#define POWER_ON_RESET             0x50          /// Power on reset
#define INIT_MEASURE               0x70          /// Init measure


#define WRITE_REG0                          0x80                   /// Write register0 operation address
#define WRITE_REG1                          0x81                   /// Write register1 operation address
#define WRITE_REG2                          0x82                   /// Write register2 operation address
#define WRITE_REG3                          0x83                   /// Write register3 operation address
#define WRITE_REG4                          0x84                   /// Write register4 operation address
#define WRITE_REG5                          0x85                   /// Write register5 operation address
#define WRITE_REG6                          0x86                   /// Write register6 operation address
#define READ_RES0                           0xB0                   /// Read 32bit result register0 address
#define READ_RES1                           0xB1                   /// Read 32bit result register1 address
#define READ_RES2                           0xB2                   /// Read 32bit result register2 address
#define READ_RES3                           0xB3                   /// Read 32bit result register3 address
#define READ_STAT                           0xB4                   /// Read 16bit state register address
#define READ_REG1                           0xB5                   /// Read 8bit what write to reg1 data, use to test communication
//#define READ_REG6                           0xB6                 /// Read register6 operation address
#define READ_IDBIT                          0xB7                   /// Read TDC ID bit(56 bits)
#define READ_PW1ST                          0xB8                   /// Read TDC PW1ST(8 bits)




/*TDC接口定义****************************/

/*
PA12 STA
PA11 DIS
PA10 INT
PA9  FIRE_IN
PA8  RSTN
*/
#define TDC_START_PIN                        GPIO_Pin_12               
#define TDC_START_GPIO_PORT                  GPIOA                     
#define TDC_START_GPIO_CLK                   RCC_AHB1Periph_GPIOA


#define TDC_EN_START_PIN                        GPIO_Pin_11               
#define TDC_EN_START_GPIO_PORT                  GPIOA                     
#define TDC_EN_START_GPIO_CLK                   RCC_AHB1Periph_GPIOA

#define TDC_FIRE_IN_PIN                        GPIO_Pin_9               
#define TDC_FIRE_IN_GPIO_PORT                  GPIOA                     
#define TDC_FIRE_IN_GPIO_CLK                   RCC_AHB1Periph_GPIOA

#define TDC_RESET_PIN                        GPIO_Pin_8               
#define TDC_RESET_GPIO_PORT                  GPIOA                    
#define TDC_RESET_GPIO_CLK                   RCC_AHB1Periph_GPIOA


#define TDC_INT_PIN                        GPIO_Pin_10               
#define TDC_INT_GPIO_PORT                  GPIOA                     
#define TDC_INT_GPIO_CLK                   RCC_AHB1Periph_GPIOA

//中断函数
#define TDC_INT_GPIO_PORT            GPIOA 
#define TDC_INT_PIN                  GPIO_Pin_10 
#define TDC_INT_EXTI_Line            EXTI_Line10
#define TDC_INT_IRQn                 EXTI15_10_IRQn

#define TDC_INT_IRQHandler           EXTI15_10_IRQHandler  

/******************************************************
*The related parameters of TDC.
******************************************************/



#define TDC_START_1                         GPIO_SetBits(TDC_START_GPIO_PORT,TDC_START_PIN);         /// TDC chip start control signal.
#define TDC_START_0                         GPIO_ResetBits(TDC_START_GPIO_PORT,TDC_START_PIN)
#define TDC_EN_START_1                      GPIO_SetBits(TDC_EN_START_GPIO_PORT,TDC_EN_START_PIN);         /// TDC chip en_start control signal.
#define TDC_EN_START_0                      GPIO_ResetBits(TDC_EN_START_GPIO_PORT,TDC_EN_START_PIN)
#define TDC_FIRE_IN_1                       GPIO_SetBits(TDC_FIRE_IN_GPIO_PORT,TDC_FIRE_IN_PIN);         /// TDC chip fire_in control signal.
#define TDC_FIRE_IN_0                       GPIO_ResetBits(TDC_FIRE_IN_GPIO_PORT,TDC_FIRE_IN_PIN)
#define TDC_RESET_1                         GPIO_SetBits(TDC_RESET_GPIO_PORT,TDC_RESET_PIN);        /// TDC chip reset control signal.
#define TDC_RESET_0                         GPIO_ResetBits(TDC_RESET_GPIO_PORT,TDC_RESET_PIN)



extern u8 INTSign;




void TDC_GPIO_Init(void);
void TDC_Init_Reg( void );
void EXTIX_Init(void);
void TDC_INT_IRQHandler(void);
void resetTDCGP22(void);
void powerOnResetTDCGP22(void);
void initMeasureTDCGP22(void);
float dotHextoDotDec(unsigned long dotHex);
void timeFlightStartTDCGP22(void);
void timeFlightRestartTDCGP22(void);
void tempStartTDCGP22(void);
void configureRegisterTDCGP22(unsigned char opcode_address,uint32_t config_reg_data);
u16  readStatusRegisterTDCGP22(void);
float readPW1STRegisterTDCGP22(void);

void readIDbytesTDCGP22(void);
u8 SPI_TDC_SendByte(u8 byte);
void calibrateResonator( void );
u32 readRegisterTDCGP22(unsigned char read_opcode_address);
void Error(void);


#endif











