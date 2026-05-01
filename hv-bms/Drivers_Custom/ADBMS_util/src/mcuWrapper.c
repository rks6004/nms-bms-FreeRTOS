/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    mcuWrapper.c
* @brief:   BMS SPI driver functions
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! \addtogroup MCU DRIVER
*  @{
*/

/*! @addtogroup Mcu Driver
*  @{
*/
#include "common.h"
#include "mcuWrapper.h"
#include "FreeRTOS.h"
#include "task.h"

#define WAKEUP_DELAY 1                         /* BMS ic wakeup delay  */

#define SPI_TIME_OUT HAL_MAX_DELAY              /* SPI Time out delay   */
#define UART_TIME_OUT HAL_MAX_DELAY             /* UART Time out delay  */
#define I2C_TIME_OUT HAL_MAX_DELAY              /* I2C Time out delay   */

#define BMS_CS_PIN SPI1_CS_Pin                      /* Mcu dependent BMS chip select        */
#define BMS_GPIO_PORT SPI1_CS_GPIO_Port             /* Mcu dependent BMS chip select port   */

SPI_HandleTypeDef *hspi         = &hspi1;       /* MUC SPI Handler      */
UART_HandleTypeDef *huart       = &huart1;      /* MUC UART Handler     */
TIM_HandleTypeDef *htim         = &htim2;       /* Mcu TIM handler */

//6830 functions

/**
 *******************************************************************************
 * Function: Delay_ms
 * @brief Delay mili second
 *
 * @details This function insert delay in ms.
 *     
 * Parameters:
 * @param [in]  delay   Delay_ms
 *
 * @return None
 *
 *******************************************************************************
*/
void Delay_ms(uint32_t delay)
{
  vTaskDelay(pdMS_TO_TICKS(delay));
}

/**
 *******************************************************************************
 * Function: adBmsCsLow
 * @brief Select chip select low
 *
 * @details This function does spi chip select low.
 *
 * @return None
 *
 *******************************************************************************
*/
void adBmsCsLow()
{
  HAL_GPIO_WritePin(BMS_GPIO_PORT, BMS_CS_PIN, GPIO_PIN_RESET);
}

/**
 *******************************************************************************
 * Function: adBmsCsHigh
 * @brief Select chip select High
 *
 * @details This function does spi chip select high.
 *
 * @return None
 *
 *******************************************************************************
*/
void adBmsCsHigh()
{
  HAL_GPIO_WritePin(BMS_GPIO_PORT, BMS_CS_PIN, GPIO_PIN_SET);
}

/**
 *******************************************************************************
 * Function: spiWriteBytes
 * @brief Writes an array of bytes out of the SPI port.
 *
 * @details This function wakeup bms ic in IsoSpi mode send dumy byte data in spi line..
 *
 * @param [in]  size            Numberof bytes to be send on the SPI line
 *
 * @param [in]  *tx_Data    Tx data pointer 
 *
 * @return None
 *
 *******************************************************************************
*/
void spiWriteBytes
( 
uint16_t size,                     /*Option: Number of bytes to be written on the SPI port*/
uint8_t *tx_Data                       /*Array of bytes to be written on the SPI port*/
)
{
  HAL_SPI_Transmit(hspi, tx_Data, size, SPI_TIME_OUT); /* SPI1 , data, size, timeout */ 
}

/**
 *******************************************************************************
 * Function: spiWriteReadBytes
 * @brief Writes and read a set number of bytes using the SPI port.
 *
 * @details This function writes and read a set number of bytes using the SPI port.
 *
 * @param [in]  *tx_data    Tx data pointer
 *
 * @param [in]  *rx_data    Rx data pointer 
 *
 * @param [in]  size            Data size 
 *
 * @return None
 *
 *******************************************************************************
*/
void spiWriteReadBytes
(
uint8_t *tx_data,                   /*array of data to be written on SPI port*/
uint8_t *rx_data,                   /*Input: array that will store the data read by the SPI port*/
uint16_t size                           /*Option: number of bytes*/
)
{
  HAL_SPI_Transmit(hspi, tx_data, 4, SPI_TIME_OUT);
  HAL_SPI_Receive(hspi, rx_data, size, SPI_TIME_OUT);
}

/**
 *******************************************************************************
 * Function: spiReadBytes
 * @brief Read number of bytes using the SPI port.
 *
 * @details This function Read a set number of bytes using the SPI port.
 *
 * @param [in]  size            Data size 
 *
 * @param [in]  *rx_data    Rx data pointer
 * 
 * @return None
 *
 *******************************************************************************
*/
void spiReadBytes(uint16_t size, uint8_t *rx_data)
{   
  HAL_SPI_Receive(hspi, rx_data, size, SPI_TIME_OUT);
}

/**
 *******************************************************************************
 * Function: startTimer()
 * @brief Start timer 
 *
 * @details This function start the timer.
 *
 * @return None
 *
 *******************************************************************************
*/
void startTimer()
{   
  //__HAL_TIM_SetCounter(htim, 0);
  HAL_TIM_Base_Start(htim);
}

/**
 *******************************************************************************
 * Function: stopTimer()
 * @brief Stop timer 
 *
 * @details This function stop the timer.
 *
 * @return None
 *
 *******************************************************************************
*/
void stopTimer()
{   
  HAL_TIM_Base_Stop(htim);
  //__HAL_TIM_SetCounter(htim, 0);
}

/**
 *******************************************************************************
 * Function: getTimCount()
 * @brief Get Timer Count Value 
 *
 * @details This function return the timer count value.
 *
 * @return tim_count
 *
 *******************************************************************************
*/
uint32_t getTimCount()
{   
  uint32_t count = 0;
  count = __HAL_TIM_GetCounter(htim);
  __HAL_TIM_SetCounter(htim, 0);
  return(count);
}


/**
 *******************************************************************************
 * Function: adBmsWakeupIc
 * @brief Wakeup bms ic using chip select
 *
 * @details This function wakeup thr bms ic using chip select.
 *
 * @param [in]  total_ic    Total_ic
 *
 * @return None
 *
 *******************************************************************************
*/
#ifndef TESTBENCH
void adBmsWakeupIc(uint8_t total_ic)
{
  for (uint8_t ic = 0; ic < total_ic; ic++)
  {
    adBmsCsLow();
    adBmsCsHigh();
    Delay_ms(WAKEUP_DELAY);
  }
}
#endif

//2950 functions


/**
 *******************************************************************************
 * Function: Delay_us
 * @brief Delay micro second
 *
 * @details This function insert delay in us.
 *
 * Parameters:
 * @param [in]	delay	Delay_us
 *
 * @return None
 *
 *******************************************************************************
*/ 
void Delay_us(uint32_t delay) 
{
  __HAL_TIM_SetCounter(htim, 0);
  __HAL_TIM_ENABLE(htim);
  while(__HAL_TIM_GetCounter(htim) < delay*64);
    __HAL_TIM_DISABLE(htim);
}

/**
 *******************************************************************************
 * Function: adBmsRawWriteRead
 * @brief write and read raw
 *
 * @details This function wakeup thr bms ic using chip select.
 *
 * @param [in]	total_ic	Total_ic
 *
 * @return None
 *
 *******************************************************************************
*/
#ifndef TESTBENCH
void adBmsRawWriteRead(uint8_t total_ic, uint8_t *tx_data, uint8_t *rx_data, uint8_t size)
{
  adBmsCsLow();
  HAL_SPI_Transmit(hspi, tx_data, 4, SPI_TIME_OUT);
  HAL_SPI_Receive(hspi, rx_data, size, SPI_TIME_OUT);
  adBmsCsHigh();
}
#endif

/**
*******************************************************************************
* Function: spiTransmit
* @brief SPI transmit bytes function with chip select control
*
* @details This function pulls the chip select low and writes given data to SPI port and pulls chip select high.
*
* @param [in]	*hspi	SPI_HandleTypeDef pointer
*
* @param [in]	*tx_data	Tx data pointer
*
* @param [in]	size	        Data size
*
* @param [in]	CS_GPIO_PORT	GPIO_TypeDef pointer (e.g.GPIOA,GPIOB)
*
* @param [in]	CS_GPIO_Pin	GPIO  pin number 
*
* @return None
*
*******************************************************************************
*/
void spiTransmit(SPI_HandleTypeDef *hspi,uint8_t *tx_data, uint16_t size,GPIO_TypeDef* CS_GPIO_PORT, uint16_t CS_GPIO_Pin)
{
  HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_Pin, GPIO_PIN_RESET);         /*!< Pull CS pin low */
  HAL_SPI_Transmit(hspi, tx_data, size, HAL_MAX_DELAY);                  /*!< Transmit data over spi */
  HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_Pin, GPIO_PIN_SET);           /*!< Pull CS pin high */

}

/**
*******************************************************************************
* Function: spiTransmitReceive
* @brief SPI transmit and receive bytes function with chip select control
*
* @details This function pulls the chip select low and writes and reads data through SPI port and pulls chip select high.
*
* @param [in]	*hspi	SPI_HandleTypeDef pointer
*
* @param [in]	*tx_data	Tx data pointer
*
* @param [in]	*rx_data	Rx data pointer
*
* @param [in]	size	        Data size
*
* @param [in]	CS_GPIO_PORT	GPIO_TypeDef pointer (e.g.GPIOA,GPIOB)
*
* @param [in]	CS_GPIO_Pin	GPIO  pin number 
*
* @return None
*
*******************************************************************************
*/
void spiTransmitReceive(SPI_HandleTypeDef *hspi,uint8_t *tx_data, uint8_t *rx_data ,uint16_t size,GPIO_TypeDef* CS_GPIO_PORT, uint16_t CS_GPIO_Pin)
{
  HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_Pin, GPIO_PIN_RESET);         /*!< Pull CS pin low */
  HAL_SPI_TransmitReceive(hspi, tx_data, rx_data, size, HAL_MAX_DELAY); /*!< Transmit and receive data over spi */
  HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_Pin, GPIO_PIN_SET);           /*!< Pull CS pin high */
}

/**
*******************************************************************************
* Function: spiReceive
* @brief SPI receive bytes function with chip select control
*
* @details This function pulls the chip select low and reads data from SPI port and pulls chip select high.
*
* @param [in]	*hspi	SPI_HandleTypeDef pointer
*
* @param [in]	*rx_data	Rx data pointer
*
* @param [in]	size	        Data size
*
* @param [in]	CS_GPIO_PORT	GPIO_TypeDef pointer (e.g.GPIOA,GPIOB)
*
* @param [in]	CS_GPIO_Pin	GPIO  pin number 
*
* @return None
*
*******************************************************************************
*/
void spiReceive(SPI_HandleTypeDef *hspi,uint8_t *rx_data, uint16_t size,GPIO_TypeDef* CS_GPIO_PORT, uint16_t CS_GPIO_Pin)
{
  HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_Pin, GPIO_PIN_RESET);         /*!< Pull CS pin low */
  HAL_SPI_Receive(hspi, rx_data, size, HAL_MAX_DELAY);                  /*!< Receive data over spi */
  HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_Pin, GPIO_PIN_SET);           /*!< Pull CS pin high */

}

/** @}*/

/** @}*/
/** @}*/