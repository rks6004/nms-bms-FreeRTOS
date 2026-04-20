/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "main.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, BMS_Fault_Pin|CHARGING_POWER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Heartbeat_GPIO_Port, Heartbeat_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CHARGING_ENABLE_Pin IMD_Fault_Input_Pin */
  GPIO_InitStruct.Pin = CHARGING_ENABLE_Pin|IMD_Fault_Input_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BMS_Fault_Pin CHARGING_POWER_Pin */
  GPIO_InitStruct.Pin = BMS_Fault_Pin|CHARGING_POWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : Heartbeat_Pin */
  GPIO_InitStruct.Pin = Heartbeat_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Heartbeat_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */

/**
 * @brief GPIO Interrupt Function callback
 * 
 * @param GPIO_Pin Pin from which the interrupt got triggered
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  // Set the set event group indicator to false
  BaseType_t pxHigherTaskWoken = pdFALSE;

  // Check which pin the interrupt was triggered from
  if (GPIO_Pin == CHARGING_ENABLE_Pin)
  {
    // Check current BMS mode
    if (bmsMode == BMS_MODE_DISCHARGE)
    {
      // Enable charging flag
      bmsMode = BMS_MODE_CHARGE;
      // Trigger flag to signal to the thread that charging is being enabled for the first time
      // to send configuration messages to the ADBMS6830
      chargingJustEnabled = CHARGE_JUST_ENABLED;
      // Set event flag to signal to charging tasks that the BMS is in charging mode and they can unblock
      xEventGroupClearBitsFromISR(charging_evt_id, EVENT_FLAG_CHARGING_DISABLE);
      // Clear event flag to signal to normal operation tasks that they should block
      xEventGroupSetBitsFromISR(charging_evt_id, EVENT_FLAG_CHARGING_ENABLE, &pxHigherTaskWoken);
    }
    else if (bmsMode == BMS_MODE_CHARGE)
    {
      // Disable charging flag
      bmsMode = BMS_MODE_DISCHARGE;
      // Trigger flag to signal to the thread that discharging is being enabled for the first time
      // to send configuration messages to the ADBMS6830
      dischargingJustEnabled = DISCHARGE_JUST_ENABLED;
      // Set event flag to signal to normal operation tasks that the BMS is in discharging mode and they can unblock
      xEventGroupClearBitsFromISR(charging_evt_id, EVENT_FLAG_CHARGING_ENABLE);
      // Clear event flag to signal to charging tasks that they should block
      xEventGroupSetBitsFromISR(charging_evt_id, EVENT_FLAG_CHARGING_DISABLE, &pxHigherTaskWoken);
    }
  }
  else if (GPIO_Pin == IMD_Fault_Input_Pin)
  {
    if (HAL_GPIO_ReadPin(IMD_Fault_Input_GPIO_Port, IMD_Fault_Input_Pin) == GPIO_PIN_SET)
    {
      encode_can_0x0c3_IMD_Fault(canBuses[VEHICLE_CAN].converter, 1);
    }
    portYIELD_FROM_ISR(pxHigherTaskWoken);
  }
}
/* USER CODE END 2 */
