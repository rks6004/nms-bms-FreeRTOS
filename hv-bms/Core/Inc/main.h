/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define INT64_TO_BYTE8(value, bytes)                                                               \
  do                                                                                               \
  {                                                                                                \
    for (int i = 0; i < 8; i++)                                                                    \
    {                                                                                              \
      (bytes)[i] = ((value) >> (i * 8)) & 0xFF;                                                    \
    }                                                                                              \
  } while (0)

#define BYTE8_TO_INT64(value, bytes)                                                               \
  do                                                                                               \
  {                                                                                                \
    (value) = (((uint64_t)(bytes)[0] << 0) | ((uint64_t)(bytes)[1] << 8) |                         \
               ((uint64_t)(bytes)[2] << 16) | ((uint64_t)(bytes)[3] << 24) |                       \
               ((uint64_t)(bytes)[4] << 32) | ((uint64_t)(bytes)[5] << 40) |                       \
               ((uint64_t)(bytes)[6] << 48) | ((uint64_t)(bytes)[7] << 56));                       \
  } while (0)

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define CHARGING_ENABLE_Pin GPIO_PIN_6
#define CHARGING_ENABLE_GPIO_Port GPIOC
#define CHARGING_ENABLE_EXTI_IRQn EXTI9_5_IRQn
#define IMD_Fault_Input_Pin GPIO_PIN_7
#define IMD_Fault_Input_GPIO_Port GPIOC
#define IMD_Fault_Input_EXTI_IRQn EXTI9_5_IRQn
#define BMS_Fault_Pin GPIO_PIN_9
#define BMS_Fault_GPIO_Port GPIOC
#define CHARGING_POWER_Pin GPIO_PIN_10
#define CHARGING_POWER_GPIO_Port GPIOC
#define Heartbeat_Pin GPIO_PIN_9
#define Heartbeat_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
