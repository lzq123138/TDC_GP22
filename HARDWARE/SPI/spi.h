#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f4xx.h"
#include <stdio.h>

/********************SPI接口定义-开头****************************/
//CLK  PA5
//MISO PA6
//MOSI PA7
//SSN  PA4

#define TDC_SPI                           SPI1
#define TDC_SPI_CLK                       RCC_APB2Periph_SPI1
#define TDC_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define TDC_SPI_SCK_PIN                   GPIO_Pin_5                  
#define TDC_SPI_SCK_GPIO_PORT             GPIOA                       
#define TDC_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define TDC_SPI_SCK_PINSOURCE             GPIO_PinSource5
#define TDC_SPI_SCK_AF                    GPIO_AF_SPI1

#define TDC_SPI_MISO_PIN                  GPIO_Pin_6                
#define TDC_SPI_MISO_GPIO_PORT            GPIOA                   
#define TDC_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define TDC_SPI_MISO_PINSOURCE            GPIO_PinSource6
#define TDC_SPI_MISO_AF                   GPIO_AF_SPI1

#define TDC_SPI_MOSI_PIN                  GPIO_Pin_7                
#define TDC_SPI_MOSI_GPIO_PORT            GPIOA                     
#define TDC_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define TDC_SPI_MOSI_PINSOURCE            GPIO_PinSource7
#define TDC_SPI_MOSI_AF                   GPIO_AF_SPI1

#define TDC_CS_PIN                        GPIO_Pin_4               
#define TDC_CS_GPIO_PORT                  GPIOA                     
#define TDC_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOA


#define TDC_CS_1                            GPIO_SetBits(TDC_CS_GPIO_PORT,TDC_CS_PIN);         /// TDC chip cs control signal.
#define TDC_CS_0                            GPIO_ResetBits(TDC_CS_GPIO_PORT,TDC_CS_PIN)

#define	DUMMY_DATA		            				0xFF




void SPI_TDC_Init(void);

u8 SPI1_ReadWriteByte(u8 TxData);

u32 SPI1_ReadWordFromReg(u8 regAddr);

u16 SPI1_Read2ByteFromReg(u8 regAddr);

u8 SPI1_ReadByteFromReg(u8 regAddr);

#endif











