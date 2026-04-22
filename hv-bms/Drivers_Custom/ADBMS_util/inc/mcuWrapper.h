/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    mcuWrapper.h
* @brief:   Generic wrapper header file
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/** @addtogroup MUC_DRIVER
*  @{
*
*/

/** @addtogroup SPI_DRIVER SPI DRIVER
*  @{
*
*/
#ifndef __ADBMSWRAPPER_H
#define __ADBMSWRAPPER_H
#include "common.h"

#include "main.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal_gpio.h"


extern SPI_HandleTypeDef hspi1;         /* Mcu dependent SPI handler */
extern UART_HandleTypeDef huart1;       /* Mcu dependent UART handler */
extern TIM_HandleTypeDef htim2;         /* Mcu dependent TIM handler */

#define CS_PIN SPI1_CS_Pin               /* Mcu dependent chip select */
#define GPIO_PORT SPI1_CS_GPIO_Port     /* Mcu dependent adc chip select port */
#endif

//6830 functions
void Delay_ms(uint32_t delay);
void adBmsCsLow(void);
void adBmsCsHigh(void);
void startTimer(void);
void stopTimer(void);
uint32_t getTimCount(void);
void adBmsWakeupIc(uint8_t total_ic);

//2950 functions
void Delay_us(uint32_t us);
void adBmsRawWriteRead(uint8_t total_ic, uint8_t *tx_data, uint8_t *rx_data, uint8_t size);
void spiTransmit(SPI_HandleTypeDef *hspi,uint8_t *tx_data, uint16_t size,GPIO_TypeDef* CS_GPIO_PORT, uint16_t CS_GPIO_Pin); //Taken from mcuFunctions.h
void spiTransmitReceive(SPI_HandleTypeDef *hspi,uint8_t *tx_data, uint8_t *rx_data ,uint16_t size,GPIO_TypeDef* CS_GPIO_PORT, uint16_t CS_GPIO_Pin); //Taken from mcuFunctions.h
void spiReceive(SPI_HandleTypeDef *hspi,uint8_t *rx_data, uint16_t size,GPIO_TypeDef* CS_GPIO_PORT, uint16_t CS_GPIO_Pin); //Taken from mcuFunctions.h

//Shared functions
void spiReadBytes(uint16_t size, uint8_t *rx_data);
void spiWriteBytes(uint16_t size, uint8_t *tx_data);
void spiWriteReadBytes(uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

/** @}*/
/** @}*/