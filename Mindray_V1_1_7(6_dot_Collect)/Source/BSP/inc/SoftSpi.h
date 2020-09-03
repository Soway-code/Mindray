/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        SoftSpi.h
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-09
** 描述:          软件模拟SPI通信
** 功能:          SPI驱动
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#ifndef __SOFTSPI_H
#define __SOFTSPI_H

#include "stm32l0xx_hal_rcc.h"
#include "stm32l0xx_hal_rcc_ex.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_gpio_ex.h"
//#include "stm32l0xx_hal_dma.h"

//SPI片选
#define SPI_NSS_PIN                                      GPIO_PIN_15
//SPI片选端口
#define SPI_NSS_PORT                                     GPIOA

//SPI MOSI引脚
#define SPI_MOSI_PIN                                     GPIO_PIN_5
//SPI MOSI端口
#define SPI_MOSI_PORT                                    GPIOB

//SPI SCK引脚
#define SPI_SCK_PIN                                      GPIO_PIN_3
//SPI SCK端口
#define SPI_SCK_PORT                                     GPIOB

//SPI MISO引脚
#define SPI_MISO_PIN                                     GPIO_PIN_4
//SPI MISO端口
#define SPI_MISO_PORT                                    GPIOB

//SPI使能
#define SPI_ENABLE                                       HAL_GPIO_WritePin(SPI_NSS_PORT, SPI_NSS_PIN, GPIO_PIN_RESET)
//SPI禁止
#define SPI_DISABLE                                      HAL_GPIO_WritePin(SPI_NSS_PORT, SPI_NSS_PIN, GPIO_PIN_SET)

//SPI MOSI置高
#define SPI_MOSI_SET                                     HAL_GPIO_WritePin(SPI_MOSI_PORT, SPI_MOSI_PIN, GPIO_PIN_SET)
//SPI MOSI置低
#define SPI_MOSI_CLR                                     HAL_GPIO_WritePin(SPI_MOSI_PORT, SPI_MOSI_PIN, GPIO_PIN_RESET)

//SPI SCK置高
#define SPI_SCK_SET                                      HAL_GPIO_WritePin(SPI_SCK_PORT, SPI_SCK_PIN, GPIO_PIN_SET)
//SPI SCK置低
#define SPI_SCK_CLR                                      HAL_GPIO_WritePin(SPI_SCK_PORT, SPI_SCK_PIN, GPIO_PIN_RESET)
//SPI MISO读取电平
#define SPI_MISO_STA                                     HAL_GPIO_ReadPin(SPI_MISO_PORT, SPI_MISO_PIN)



void SPI_Init(void);
void SPI_Bit_Set(void);
void SPI_Bit_Clr(void);
void Delay_us(uint32_t us);
uint8_t SPI_Receive_8Bit(void);
uint32_t SPI_Receive_24Bit(void);
void SPI_Send_8Bit(uint8_t sBit8);
void SPI_Send_16Bit(uint16_t sBit16);
void SPI_Send_32Bit(uint32_t sBit32);

#endif
