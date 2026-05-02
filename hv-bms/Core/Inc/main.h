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

#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

// Flags to synchronize RTOS tasks based on charging or discharging operation of the BMS
#define EVENT_FLAG_CHARGING_DISABLE 0x01U
#define EVENT_FLAG_CHARGING_ENABLE  0x02U

// Flags to keep track of current BMS operating mode (charge vs. discharge)
#define BMS_MODE_DISCHARGE 0
#define BMS_MODE_CHARGE    1

// Flags to keep track of when discharge mode is enabled to send configuration registers to ADBMS6830
#define DISCHARGE_REGULAR_OPERATION 0
#define DISCHARGE_JUST_ENABLED      1

// Flags to keep track of when charge mode is enabled to send configuration registers to ADBMS6830
#define CHARGE_REGULAR_OPERATION 0
#define CHARGE_JUST_ENABLED      1

#define ADBMS_6830_IC_NUM 7
#define ADBMS_2950_IC_NUM 1

// Cell maximum settings
#define MAX_CELL_VOLTAGE 4.2
#define MIN_CELL_VOLTAGE 2.7
#define MAX_TEMP         60
#define MIN_TEMP         -20
#define MAX_CURRENT      600

// Charging logic variables
#define TAPER_CURRENT 0.150 // End of charge current in Amps
#define CELL_COUNT                                                                                 \
  ((ADBMS_6830_IC_NUM - ADBMS_2950_IC_NUM) * CELL) // Example cell count, adjust as necessary
#define CHARGING_TEMP_THRESH     45
#define FULL_CHARGE_RATE_OK_VOLT 3.2
#define FULL_CHARGE_RATE_OK_TEMP 20
#define CHARGE_START_OK_TEMP     5
#define CHARGE_CV_VALUE          (MAX_CELL_VOLTAGE - 0.015)

// PEC Error Added
#define PEC_ERROR_LIMIT 5

/* Includes ------------------------------------------------------------------*/
#ifndef TESTBENCH
#include "stm32f4xx_hal.h"
#endif

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern SemaphoreHandle_t bmsMutexHandle;
extern TimerHandle_t faultLatchTimerHandle;

#ifdef TESTBENCH
extern SemaphoreHandle_t ioMutexHandle;
extern EventGroupHandle_t testbench_datastream_ready;
extern EventGroupHandle_t testbench_evt_id;
extern EventBits_t expected_test_bits;
#endif

// Flag to keep track of charging enabled
extern EventGroupHandle_t charging_evt_id;
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
#ifndef TESTBENCH
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
#endif
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
