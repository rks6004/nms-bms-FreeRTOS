/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* USER CODE BEGIN Includes */
#include "bms_util.h"
#include "adBms2950Driver.h"
#include "adBms6830Driver.h"
#include "adBms6830PrintResult.h"
#include "adBmsCommonDriver.h"

#ifndef TESTBENCH

#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#else
#include "adBms6830_TESTBENCH.h"
#endif

#include "main.h"

//should pull from appropriate header areas depending on TESTBENCH flag in Makefile
#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"
#include "util.h"

#include <stdio.h>
/* USER CODE END Includes */


/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



// 5 minutes in milliseconds
#define DCTO_TICKS pdMS_TO_TICKS(5 * 60 * 1000)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#ifndef TESTBENCH
#define SET_FAULT_SIG(fault, ic)                                                                   \
  do                                                                                               \
  {                                                                                                \
    taskENTER_CRITICAL();                                                                          \
    switch (ic)                                                                                    \
    {                                                                                              \
      case 1:                                                                                      \
        encode_can_0x0c3_SEG1##fault(canBuses[VEHICLE_CAN].converter, 1);                          \
        break;                                                                                     \
      case 2:                                                                                      \
        encode_can_0x0c3_SEG2##fault(canBuses[VEHICLE_CAN].converter, 1);                          \
        break;                                                                                     \
      case 3:                                                                                      \
        encode_can_0x0c3_SEG3##fault(canBuses[VEHICLE_CAN].converter, 1);                          \
        break;                                                                                     \
      case 4:                                                                                      \
        encode_can_0x0c3_SEG4##fault(canBuses[VEHICLE_CAN].converter, 1);                          \
        break;                                                                                     \
      case 5:                                                                                      \
        encode_can_0x0c3_SEG5##fault(canBuses[VEHICLE_CAN].converter, 1);                          \
        break;                                                                                     \
      case 6:                                                                                      \
        encode_can_0x0c3_SEG6##fault(canBuses[VEHICLE_CAN].converter, 1);                          \
        break;                                                                                     \
    }                                                                                              \
    taskEXIT_CRITICAL();                                                                           \
    bmsFault();                                                                                    \
  } while (0);                                                                                     
#else
#define SET_FAULT_SIG(fault, ic)                                                                   \
  do                                                                                               \
  {                                                                                                \
    taskENTER_CRITICAL();                                                                          \
    bmsFault();                                                                                    \ 
    taskEXIT_CRITICAL();                                                                           \
    xSemaphoreTake(ioMutexHandle, portMAX_DELAY);                                                  \
    printf("Fault detected at SEGMENT: %d\n", ic);                                                 \
    xSemaphoreGive(ioMutexHandle);                                                                 \
  } while (0);
#endif
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
cell_asic_6830 ADBMS_6830_IC[ADBMS_6830_IC_NUM];

#if (ADBMS_2950_IC_NUM > 0)
cell_asic_2950 ADBMS_2950_IC[ADBMS_2950_IC_NUM];
#endif // ADBMS_2950_IC_NUM > 0


#ifdef TESTBENCH
extern emulated_adbms_6830 characteristic_6830[ADBMS_6830_IC_NUM];
extern emulated_adbms_2950 characteristic_2950[ADBMS_2950_IC_NUM];
#endif //TESTBENCH


uint8_t acellPecErrorCount[ADBMS_6830_IC_NUM] = {0};
uint8_t auxPecErrorCount[ADBMS_6830_IC_NUM] = {0};

// Enum for charge state
typedef enum
{
  CHARGE_STATE_SLOW_CHARGE = 1,
  CHARGE_STATE_FAST_CHARGE,
  CHARGE_STATE_CONSTANT_VOLTAGE,
  CHARGE_STATE_DONE,
  CHARGE_STATE_ERROR
} ChargingState;

// Enum for if charge is enbaled
typedef enum
{
  CHARGE_ENABLED = 0,
  CHARGE_DISABLED = 1
} ChargeEnableState;

int16_t chargeCurrent; //A
ChargingState chargeState;
ChargeEnableState chargerEnable;


SemaphoreHandle_t bmsMutexHandle;
SemaphoreHandle_t ioMutexHandle;
TimerHandle_t faultLatchTimerHandle;


// Flag to keep track of charging enabled
EventGroupHandle_t charging_evt_id;

int bmsMode = BMS_MODE_DISCHARGE;
int chargingJustEnabled = CHARGE_REGULAR_OPERATION;
int dischargingJustEnabled = DISCHARGE_REGULAR_OPERATION;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
#ifndef TESTBENCH
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
#endif
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{ 
  #ifdef __GNUC__
    //since we are runnning the testbench, we can use OS-focused commands
    #ifdef TESTBENCH
    int bytes_written = printf("%c", ch);
    return ch ? bytes_written > 0 : bytes_written;
    #else
    #error PUTCHAR_PROTOTYPE not implemented for non-testbench use
    #endif
  #else
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
  #endif
}

#ifdef __GNUC__
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif

GETCHAR_PROTOTYPE
{
  uint8_t ch = 0;
  #ifdef __GNUC__
    //since we are runnning the testbench, we can use OS-focused commands
    #ifdef TESTBENCH
    ch = getchar();
    #else
    #error GETCHAR_PROTOTYPE not implemented for non-testbench use
    #endif
  #else
  /* Clear the Overrun flag just before receiving the first character */
  __HAL_UART_CLEAR_OREFLAG(&huart1);

  /* Wait for reception of a character on the USART RX line and echo this
   * character on console */
  HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
  #endif
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief 
 */
void chargeController(void)
{
  chargeCurrent = 0; //A
  chargeState = CHARGE_STATE_SLOW_CHARGE;
  chargerEnable = CHARGE_ENABLED;

  // Local varibles used in function
  int16_t chargeVoltage = (CELL_COUNT * MAX_CELL_VOLTAGE * 10); // mV

  // Create data variables voltage
  double maxVoltage = 0;
  double minVoltage = 0;

  // Create data variables temp
  double maxTemperature = 0;
  double minTemperature = 0;

  // Take mutex to safely access shared BMS data
  taskENTER_CRITICAL();
  #ifndef TESTBENCH
  decode_can_0x4c4_Max_Voltage(canBuses[VEHICLE_CAN].converter, &maxVoltage);
  decode_can_0x4c4_Min_Voltage(canBuses[VEHICLE_CAN].converter, &minVoltage);
  decode_can_0x4c5_Max_Temperature(canBuses[VEHICLE_CAN].converter, &maxTemperature);
  decode_can_0x4c5_Min_Temperature(canBuses[VEHICLE_CAN].converter, &minTemperature);
  #else
  maxVoltage = MAX_CELL_VOLTAGE - 0.5;
  minVoltage = maxVoltage - 0.5; 
  #endif
  taskEXIT_CRITICAL();

  // Check for faults
  if (minVoltage < MIN_CELL_VOLTAGE || maxVoltage > MAX_CELL_VOLTAGE ||
      minTemperature < CHARGE_START_OK_TEMP || maxTemperature > CHARGING_TEMP_THRESH)
  {
    chargeCurrent = 0;
    chargerEnable = CHARGE_DISABLED;
    chargeState = CHARGE_STATE_ERROR;
  }

  // Charge states
  // 1 = slow charge CC, 2 = fast charge CC, 3 = CV, 4 = Charge Done, 5 = error
  switch (chargeState)
  {
    /*
    The slow charge state charges at 5A
    This state slowly brings our cell voltages up by applying a constant current
    If the smallest cell voltage and minimum termperature are greater than the safe values, switch to fast charge mode
    */
    case CHARGE_STATE_SLOW_CHARGE:
      if (minVoltage > FULL_CHARGE_RATE_OK_VOLT && minTemperature > FULL_CHARGE_RATE_OK_TEMP)
      {
        chargeState = CHARGE_STATE_FAST_CHARGE;
        chargeCurrent = 50;
        break;
      }
      if (maxVoltage >= CHARGE_CV_VALUE)
      {
        chargeState = CHARGE_STATE_CONSTANT_VOLTAGE;
        chargeCurrent = 50;
        break;
      }
      // Current of 5A
      chargeCurrent = 50;
      break;
    /*
    The fast charge state charges at 10A
    This state quickly brings our cell voltages to their maximum value by applying a constant current
    If the highest cell voltage reaches the threshold, switch from constant current to constant voltage mode
    */
    case CHARGE_STATE_FAST_CHARGE:
      // If we get to 4.195V, go to CV mode
      if (maxVoltage >= CHARGE_CV_VALUE)
      {
        chargeState = CHARGE_STATE_CONSTANT_VOLTAGE;
        chargeCurrent = 100;
        break;
      }
      // Current of 10A
      chargeCurrent = 100;
      break;
    /*
    The constant voltage charge state charges at a dynamic PI-controlled current
    This state brings all other cells to the maximum threshold and decreases the current magnitude
    Charging is finished when the smallest cell voltage is within 10mV of the voltage threshold
    */
    case CHARGE_STATE_CONSTANT_VOLTAGE:
      if (chargeCurrent < TAPER_CURRENT && minVoltage > (CHARGE_CV_VALUE - 0.010))
      {
        chargeCurrent = 0;
        chargeState = CHARGE_STATE_DONE;
        break;
      }
      // PI control loop
      if (maxVoltage >= CHARGE_CV_VALUE)
      {
        //Step down the charge current
        chargeCurrent = (int16_t)(chargeCurrent * 0.9);
        break;
      }
      break;
    // Set charge current to 0 when charge finished
    case CHARGE_STATE_DONE:
      chargeCurrent = 0;
      break;
    // In the case of an error, set charge current to 0 and disable charger
    case CHARGE_STATE_ERROR:
      chargeCurrent = 0;
      chargerEnable = CHARGE_DISABLED;
      break;
  }

  // Send CAN messages
  #ifndef TESTBENCH
  taskENTER_CRITICAL();
  encode_can_0x1806e5f4_Charger_Max_Voltage(canBuses[CHARGER_CAN].converter, (chargeVoltage));
  encode_can_0x1806e5f4_Charger_Max_Current(canBuses[CHARGER_CAN].converter, (chargeCurrent));
  encode_can_0x1806e5f4_Charger_Control(canBuses[CHARGER_CAN].converter, (chargerEnable));
  taskEXIT_CRITICAL();

  packCanTx(CAN_ID_BMS_CHARGER_CONTROL, CHARGER_CAN);
  #else
  #endif
}

/**
 * @brief Pulls hardware latches during a fault
 */
void bmsFaultLatch(void)
{
  #ifndef TESTBENCH
  HAL_GPIO_WritePin(BMS_Fault_GPIO_Port, BMS_Fault_Pin, GPIO_PIN_RESET);
  #else
  xSemaphoreTake(ioMutexHandle, portMAX_DELAY);
  printf("BMS Fault Latch Pulled!\n");
  xSemaphoreGive(ioMutexHandle);
  #endif
}

/**
 * @brief Sends faults over CAN and pulls latches if inverter disabled
 */
void bmsFault(void)
{
  uint8_t inverterState = 0;

  taskENTER_CRITICAL();
  #ifndef TESTBENCH
  decode_can_0x0c0_Inverter_Enable(canBuses[VEHICLE_CAN].converter, &inverterState);
  #endif
  taskEXIT_CRITICAL();
  
  #ifdef TESTBENCH
  xSemaphoreTake(ioMutexHandle, portMAX_DELAY);
  printf("Inverter Disabled due to BMS Fault\n");
  xSemaphoreGive(ioMutexHandle);
  #endif

  if (inverterState == 0)
  {
    bmsFaultLatch();
    // If inverter not disabled, wait for it to be disabled
  }
  else if (inverterState == 1 && xTimerIsTimerActive(faultLatchTimerHandle))
  {
    xTimerStart(faultLatchTimerHandle, 0);
  }
}

/**
 * @brief BMS callback function for creation of fault latch timer
 */
void bmsFaultHandle(TimerHandle_t xTimer)
{
  bmsFaultLatch();
}

/**
 * @brief Read UV and OV registers status
 */
void adBms6830_cell_ov_uv_check(void)
{
  float voltage = 0.0;
  cell_asic_6830 *ic = &ADBMS_6830_IC[0];
  // Parse through all registers to check for OV or UV
  // Start at 1 to skip pack monitor
  for (uint8_t curr_ic = ADBMS_2950_IC_NUM; curr_ic < ADBMS_6830_IC_NUM; curr_ic++)
  {
    // Cell PEC Error check
    if (acellPecErrorCount[curr_ic])
    {
      continue;
    }

    // Iterate through cell registers
    for (int i = 0; i < 16; i++)
    {
      voltage = getVoltage(ic[curr_ic].acell.ac_codes[i]);

      // OV
      if (voltage > MAX_CELL_VOLTAGE)
      {
        SET_FAULT_SIG(OV, curr_ic);
        bmsFault();
      }

      // UV
      if (voltage < MIN_CELL_VOLTAGE)
      {
        SET_FAULT_SIG(UV, curr_ic);
        bmsFault();
      }
    }
  }
}

/**
 * @brief Check for cell OT and UT
 */
void adBms6830_cell_ot_ut_check(void)
{
  float temperature = 0.0;
  cell_asic_6830 *ic = &ADBMS_6830_IC[0];
  for (int curr_ic = ADBMS_2950_IC_NUM; curr_ic < ADBMS_6830_IC_NUM; curr_ic++)
  {
    // Aux PEC Error for temps
    if (auxPecErrorCount[curr_ic])
    {
      continue;
    }

    // Iterate through cell registers
    for (int temp = 0; temp < (AUX - 2); temp++)
    {
      temperature = ic[curr_ic].cell_temperatures.cell_temps[temp];

      // OT
      if (temperature > MAX_TEMP)
      {
        SET_FAULT_SIG(OT, curr_ic);
        bmsFault();
      }

      // UT
      if (temperature < MIN_TEMP && temperature > -50.0)
      {
        SET_FAULT_SIG(UT, curr_ic);
        bmsFault();
      }
    }
  }
}

#if (ADBMS_2950_IC_NUM > 0)
/**
 * @brief Function that polls the pack monitor for pack over-current
 */
void adbms2950_pack_oc_check(void)
{

  // Create data variables
  float current = 0.0;
  cell_asic_2950 *IC = &ADBMS_2950_IC[0];

  // Get current
  current = getPackCurrent(ADBMS_2950_IC_NUM, IC);

  // Check for fault
  if (current > MAX_CURRENT)
  {
    taskENTER_CRITICAL();
    #ifndef TESTBENCH
    encode_can_0x0c3_PACKOC(canBuses[VEHICLE_CAN].converter, 1);
    #endif
    taskEXIT_CRITICAL();
    bmsFault();
  }
}
#endif // ADBMS_2950_IC_NUM > 0

/**
  * @brief  Checks the BMS for any cell or pack faults and transmits fault information over CAN.
  * @param  argument: None
  * @retval None
  */
void checkBMSFaults(void)
{
  /* Infinite loop */
  if (xSemaphoreTake(bmsMutexHandle, pdMS_TO_TICKS(10)))
  {
    // Check for cell overvolt or undervolt faults
    adBms6830_cell_ov_uv_check();
    // Check for cell overtemp or undertemp faults
    adBms6830_cell_ot_ut_check();

#if (ADBMS_2950_IC_NUM > 0)
    //Check for pack overcurrent faults
    adbms2950_pack_oc_check();
#endif // ADBMS_2950_IC_NUM > 0

    xSemaphoreGive(bmsMutexHandle);
  }

  #ifndef TESTBENCH
  packCanTx(CAN_ID_BMS_FAULT, VEHICLE_CAN);
  #else
  #endif
}

/**
 * @brief Obtains voltage data from each segment
 */
void adBms6830_get_voltage_data(void)
{
  // Create data variables
  cell_asic_6830 *ic = &ADBMS_6830_IC[0];
  float maxVoltage = getVoltage(ic[ADBMS_2950_IC_NUM].acell.ac_codes[0]);
  float minVoltage = getVoltage(ic[ADBMS_2950_IC_NUM].acell.ac_codes[0]);
  float avgVoltage = 0.0;
  float voltage = 0.0;

  // Iterate through segments
  for (int curr_ic = ADBMS_2950_IC_NUM; curr_ic < ADBMS_6830_IC_NUM; curr_ic++)
  {
    // Cell PEC Error Check
    if (acellPecErrorCount[curr_ic])
    {
      continue;
    }

    // Iterate through cells
    for (int i = 0; i < 16; i++)
    {
      // Obtain voltage
      voltage = getVoltage(ic[curr_ic].acell.ac_codes[i]);

      // Check for max
      if (voltage > maxVoltage)
      {
        maxVoltage = voltage;
        taskENTER_CRITICAL();
        #ifndef TESTBENCH
        encode_can_0x4c4_Max_Voltage_Location(canBuses[VEHICLE_CAN].converter,
                                              ((curr_ic - ADBMS_2950_IC_NUM) * 16) + i + 1);
        #else
        #endif
        taskEXIT_CRITICAL();
      }

      // Check for min
      if (voltage < minVoltage)
      {
        minVoltage = voltage;
        taskENTER_CRITICAL();
        #ifndef TESTBENCH
        encode_can_0x4c4_Min_Voltage_Location(canBuses[VEHICLE_CAN].converter,
                                              ((curr_ic - ADBMS_2950_IC_NUM) * 16) + i + 1);
        #else
        #endif
        taskEXIT_CRITICAL();
      }

      avgVoltage += voltage;
    }
  }

#if (ADBMS_2950_IC_NUM <= 0)
  // Without pack monitor, use the sum of all cell voltages to compute pack data
  taskENTER_CRITICAL();
  #ifndef TESTBENCH
  encode_can_0x4c6_Pack_Voltage(canBuses[VEHICLE_CAN].converter, (double)avgVoltage);
  encode_can_0x4c6_Pack_Current(canBuses[VEHICLE_CAN].converter, (double)0);
  #else
  #endif
  taskEXIT_CRITICAL();
#endif // ADBMS_2950_IC_NUM <= 0

  // Calculate average
  avgVoltage *= 1.0 / ((double)((ADBMS_6830_IC_NUM - ADBMS_2950_IC_NUM) * 16.0));

  // Send max, min, and average over CAN
  taskENTER_CRITICAL();
  #ifndef TESTBENCH
  encode_can_0x4c4_Max_Voltage(canBuses[VEHICLE_CAN].converter, (double)maxVoltage);
  encode_can_0x4c4_Min_Voltage(canBuses[VEHICLE_CAN].converter, (double)minVoltage);
  encode_can_0x4c4_Avg_Voltage(canBuses[VEHICLE_CAN].converter, (double)avgVoltage);
  #else
  #endif
  taskEXIT_CRITICAL();
}

void adBms6830_get_temperature_data(void)
{
  // Create data variables
  cell_asic_6830 *ic = &ADBMS_6830_IC[0];
  float maxTemperature = ic[ADBMS_2950_IC_NUM].cell_temperatures.cell_temps[0];
  float minTemperature = ic[ADBMS_2950_IC_NUM].cell_temperatures.cell_temps[0];
  float avgTemperature = 0.0;
  float temperature = 0.0;

  // Iterate through segments
  for (int curr_ic = ADBMS_2950_IC_NUM; curr_ic < ADBMS_6830_IC_NUM; curr_ic++)
  {
    // Temps PEC Error Check
    if (auxPecErrorCount[curr_ic])
    {
      continue;
    }

    // Iterate through cells
    for (int temp = 0; temp < (AUX - 2); temp++)
    {
      // Obtain temperature
      temperature = ic[curr_ic].cell_temperatures.cell_temps[temp];

      // Check for max
      if (temperature > maxTemperature)
      {
        maxTemperature = temperature;
        taskENTER_CRITICAL();
        #ifndef TESTBENCH
        encode_can_0x4c5_Max_Temperature_Location(canBuses[VEHICLE_CAN].converter,
                                                  ((curr_ic - ADBMS_2950_IC_NUM) * 16) + temp + 1);
        #else
        #endif
        taskEXIT_CRITICAL();
      }

      // Check for min
      if (temperature < minTemperature && temperature > -50.0)
      {
        minTemperature = temperature;
        taskENTER_CRITICAL();
        #ifndef TESTBENCH
        encode_can_0x4c5_Min_Temperature_Location(canBuses[VEHICLE_CAN].converter,
                                                  ((curr_ic - ADBMS_2950_IC_NUM) * 16) + temp + 1);
        #else
        #endif
        taskEXIT_CRITICAL();
      }

      avgTemperature += temperature;
    }
  }

  // Calculate average
  avgTemperature *= 1.0 / ((double)((ADBMS_6830_IC_NUM - ADBMS_2950_IC_NUM) * 16.0));

  // Send max, min, and average over CAN
  taskENTER_CRITICAL();
  #ifndef TESTBENCH
  encode_can_0x4c5_Max_Temperature(canBuses[VEHICLE_CAN].converter, (double)maxTemperature);
  encode_can_0x4c5_Min_Temperature(canBuses[VEHICLE_CAN].converter, (double)minTemperature);
  encode_can_0x4c5_Avg_Temperature(canBuses[VEHICLE_CAN].converter, (double)avgTemperature);
  #else
  #endif
  taskEXIT_CRITICAL();
}

#if (ADBMS_2950_IC_NUM > 0)
void adBms2950_get_data(void)
{
  // Create data variables
  cell_asic_2950 *ic = &ADBMS_2950_IC[0];
  float voltage = getPackVoltage(ADBMS_2950_IC_NUM, ic);
  float current = getPackCurrent(ADBMS_2950_IC_NUM, ic);

  // Send pack data over CAN
  taskENTER_CRITICAL();
  #ifndef TESTBENCH
  encode_can_0x4c6_Pack_Voltage(canBuses[VEHICLE_CAN].converter, (double)voltage);
  encode_can_0x4c6_Pack_Current(canBuses[VEHICLE_CAN].converter, (double)current);
  #else
  #endif
  taskEXIT_CRITICAL();
}
#endif // ADBMS_2950_IC_NUM > 0

void checkAcellPEC(void)
{
  for (int curr_ic = ADBMS_2950_IC_NUM; curr_ic < ADBMS_6830_IC_NUM; curr_ic++)
  {
    if (ADBMS_6830_IC[curr_ic].cccrc.acell_pec != 0)
    {
      acellPecErrorCount[curr_ic]++;
      if (acellPecErrorCount[curr_ic] == PEC_ERROR_LIMIT)
      {
        taskENTER_CRITICAL();
        #ifndef TESTBENCH
        encode_can_0x0c3_PEC_ERROR(canBuses[VEHICLE_CAN].converter, 1);
        #endif
        taskEXIT_CRITICAL();
        #ifdef TESTBENCH
        xSemaphoreTake(ioMutexHandle, portMAX_DELAY);
        printf("AvgCell PEC Error @ Segment: %d", curr_ic);
        xSemaphoreGive(ioMutexHandle);
        #endif
        bmsFault();
      }
    }
    else
    {
      acellPecErrorCount[curr_ic] = 0;
    }
  }
}

void checkAuxPEC(void)
{
  for (int curr_ic = ADBMS_2950_IC_NUM; curr_ic < ADBMS_6830_IC_NUM; curr_ic++)
  {
    if (ADBMS_6830_IC[curr_ic].cccrc.aux_pec != 0)
    {
      auxPecErrorCount[curr_ic]++;
      if (auxPecErrorCount[curr_ic] == PEC_ERROR_LIMIT)
      {
        taskENTER_CRITICAL();
        #ifndef TESTBENCH
        encode_can_0x0c3_PEC_ERROR(canBuses[VEHICLE_CAN].converter, 1);
        #endif
        taskEXIT_CRITICAL();
        #ifdef TESTBENCH
        xSemaphoreTake(ioMutexHandle, portMAX_DELAY);
        printf("Aux PEC Error @ Segment: %d", curr_ic);
        xSemaphoreGive(ioMutexHandle);
        #endif
        bmsFault();
      }
    }
    else
    {
      auxPecErrorCount[curr_ic] = 0;
    }
  }
}

/**
  * @brief  Transmits BMS data over CAN.
  * @param  argument: UNUSED pointer passed by thread creation
  * @retval None
  */
void bmsDataCanTxTask(void *argument)
{
  // Infinite loop
  for (;;)
  {
    #ifndef TESTBENCH
    packCanTx(CAN_ID_BMS_VOLTAGE_DATA, VEHICLE_CAN);
    packCanTx(CAN_ID_BMS_TEMPERATURE_DATA, VEHICLE_CAN);
    packCanTx(CAN_ID_BMS_PACK_DATA, VEHICLE_CAN);
    #else
    #endif

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

/**
  * @brief  Function that manages battery measurements during discharge.
  * @param  argument: UNUSED pointer passed by thread creation
  * @retval None
  */
void dischargingTask(void *argument)
{
  /* Infinite loop */
  for (;;)
  {
    // Wait until notification that BMS is in discharging mode
    // This thread should be in BLOCKED status
    xEventGroupWaitBits(charging_evt_id,
                        EVENT_FLAG_CHARGING_DISABLE,
                        pdFALSE,
                        pdTRUE,
                        portMAX_DELAY);
    if (dischargingJustEnabled == DISCHARGE_JUST_ENABLED)
    {
      if (xSemaphoreTake(bmsMutexHandle, pdMS_TO_TICKS(10)))
      {
        // Initialize ADBMS6830 configuration for discharge mode
        adBms6830_init_config(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);

#if (ADBMS_2950_IC_NUM > 0) // Conditional init dependent on number of pack monitors
        // Initialize ADBMS2950 configuration
        adBms2950_init_config(ADBMS_2950_IC_NUM, &ADBMS_2950_IC[0]);
        #ifndef TESTBENCH //no need to send configuration to emulated registers
        // Write configurations to both chips
        adBmsCommonWriteConfig(ADBMS_6830_IC_NUM,
                               ADBMS_2950_IC_NUM,
                               &ADBMS_6830_IC[0],
                               &ADBMS_2950_IC[0]);
        #endif
#else  // !(ADBMS_2950_IC_NUM > 0)
        #ifndef TESTBENCH //no need to send configuration to emulated registers
        adBmsWakeupIc(ADBMS_6830_IC_NUM);
        adBms6830WriteData(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0], WRCFGA, Config, A);
        adBms6830WriteData(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0], WRCFGB, Config, B);
        #endif
#endif // (ADBMS_2950_IC_NUM > 0)
        #ifndef TESTBENCH //no need to send configuration to emulated registers
        // Function call to send commands to the ADBMS6830 to initialize continuous measurement
        adBms6830_init_measurements(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);
        #endif
        // Reset dischargingJustEnabled flag as the configuration only needs to be set once
        // Only move on to regular operation after we successfully configured the ICs
        dischargingJustEnabled = DISCHARGE_REGULAR_OPERATION;
        xSemaphoreGive(bmsMutexHandle);
      }
    }
    if (xSemaphoreTake(bmsMutexHandle, pdMS_TO_TICKS(10)))
    {
      #ifndef TESTBENCH
      adBmsWakeupIc(ADBMS_6830_IC_NUM);
      #endif
      // Measure all average cell voltage registers
      adBms6830_read_avgcell_voltages(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);
      #ifndef TESTBENCH
      // Start auxiliary voltage measurements
      adBms6830_start_aux_voltage_measurment(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);
      #endif
      // Read auxiliary voltages for thermistors
      adBms6830_read_aux_voltages(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);
      // Convert thermistor voltage to temperature
      adBms6830_populate_cell_temps(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);

#if (ADBMS_2950_IC_NUM > 0)
      // Read pack voltage and current from ADBMS2950
      adBms2950_read_acc_ivbat(ADBMS_2950_IC_NUM, &ADBMS_2950_IC[0]);
#endif // ADBMS_2950_IC_NUM > 0

      // Check Cell PEC Errors
      checkAcellPEC();
      // Check Temp PEC Errors
      checkAuxPEC();
      // Send cell voltage data over CAN
      adBms6830_get_voltage_data();
      // Send cell temperature data over CAN
      adBms6830_get_temperature_data();

#if (ADBMS_2950_IC_NUM > 0)
      // Send pack monitor data over CAN
      adBms2950_get_data();
#endif // ADBMS_2950_IC_NUM > 0

    xSemaphoreGive(bmsMutexHandle);
    }
    // Check for BMS Faults
    checkBMSFaults();
  }
}

/**
  * @brief  Function that manages BMS measurements and cell balancing during charging.
  * @param  argument: UNUSED pointer passed by thread creation
  * @retval None
  */
void chargingTask(void *argument)
{
  TickType_t lastConfigUpdate = 0;
  /* Infinite loop */
  for (;;)
  {
    // Wait until notification that BMS is in charging mode
    // This thread should be in BLOCKED status
    xEventGroupWaitBits(charging_evt_id,
                        EVENT_FLAG_CHARGING_ENABLE,
                        pdFALSE,
                        pdTRUE,
                        portMAX_DELAY);

    // Force reconfiguration if we get close to the timeout to extend it
    if ((xTaskGetTickCount() - lastConfigUpdate) > DCTO_TICKS)
    {
      chargingJustEnabled = CHARGE_JUST_ENABLED;
    }

    // Check if the task just got enabled
    if (chargingJustEnabled == CHARGE_JUST_ENABLED)
    {
      if (xSemaphoreTake(bmsMutexHandle, pdMS_TO_TICKS(10)))
      {
        // Create charging configuration for ADBMS6830
        adBms6830_init_charging_config(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);

#if (ADBMS_2950_IC_NUM > 0)
        // Write configurations to both chips
        adBmsCommonWriteConfig(ADBMS_6830_IC_NUM,
                               ADBMS_2950_IC_NUM,
                               &ADBMS_6830_IC[0],
                               &ADBMS_2950_IC[0]);
#else  // !(ADBMS_2950_IC_NUM > 0)
        adBmsWakeupIc(ADBMS_6830_IC_NUM);
        adBms6830WriteData(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0], WRCFGA, Config, A);
        adBms6830WriteData(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0], WRCFGB, Config, B);
#endif // !(ADBMS_2950_IC_NUM > 0)

        // Function call to send commands to the ADBMS_6830_IC to initialize continuous measurement
        adBms6830_init_charging_measurements(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);
        lastConfigUpdate = xTaskGetTickCount();

        // Reset chargingJustEnabled flag as the configuration only needs to be set once
        // Only move on to regular operation after we successfully configured the ICs
        chargingJustEnabled = CHARGE_REGULAR_OPERATION;

        xSemaphoreGive(bmsMutexHandle);
      }
    }
    if (xSemaphoreTake(bmsMutexHandle, pdMS_TO_TICKS(10)))
    {
      // Measure all average cell voltage registers
      adBms6830_charging_voltage_measurement(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);

      // STOP BALANCING WHEN WE HAVE A FAULT
      uint64_t bmsFaults = 0;
      taskENTER_CRITICAL();
      #ifndef TESTBENCH
      pack_message_vehicle(canBuses[VEHICLE_CAN].converter, CAN_ID_BMS_FAULT, &bmsFaults);
      #endif
      taskEXIT_CRITICAL();

      if (bmsFaults != 0)
      {
        adBms6830WriteData(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0], WRPWMA, Pwm, A);
        adBms6830WriteData(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0], WRPWMB, Pwm, B);
      }
      else
      {
        // Balance cells algorithm
        adBms6830_balance_cells(ADBMS_6830_IC_NUM, ADBMS_2950_IC_NUM, &ADBMS_6830_IC[0]);
      }

      adBms6830_start_aux_voltage_measurment(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);
      // Read auxiliary voltages for thermistors
      adBms6830_read_aux_voltages(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);
      // Convert thermistor voltage to temperature
      adBms6830_populate_cell_temps(ADBMS_6830_IC_NUM, &ADBMS_6830_IC[0]);

#if (ADBMS_2950_IC_NUM > 0)
      // Read pack voltage and current from ADBMS2950
      adBms2950_read_acc_ivbat(ADBMS_2950_IC_NUM, &ADBMS_2950_IC[0]);
#endif // ADBMS_2950_IC_NUM > 0

      // Check Cell PEC Errors
      checkAcellPEC();
      // Check Temp PEC Errors
      checkAuxPEC();
      // Send cell voltage data over CAN
      adBms6830_get_voltage_data();
      // Send cell temperature data over CAN
      adBms6830_get_temperature_data();

#if (ADBMS_2950_IC_NUM > 0)
      // Send pack monitor data over CAN
      adBms2950_get_data();
#endif // ADBMS_2950_IC_NUM > 0

      // UART PRINTS
      printVoltages(ADBMS_6830_IC_NUM, ADBMS_2950_IC_NUM, &ADBMS_6830_IC[0], AvgCell);
      printCellTemperatures(ADBMS_6830_IC_NUM, ADBMS_2950_IC_NUM, &ADBMS_6830_IC[0]);

      xSemaphoreGive(bmsMutexHandle);
    }
    checkBMSFaults();

    // Call charge controller
    chargeController();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
/**
 * @brief Initialization of hardware perihperals: GPIO, SPI, UART, Timer 2, 
 * and the CAN interface.
 * 
 * Additionally drives BMS fault pin high and configures standard input stream 
 * to operate without buffering.
 * @param None
 * @retval None
 */
void peripheralsInit(void)
{
  #ifndef TESTBENCH
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  HAL_GPIO_WritePin(BMS_Fault_GPIO_Port, BMS_Fault_Pin, GPIO_PIN_SET);
  canInit(CAN_TX_QUEUE_SIZE, CAN_RX_QUEUE_SIZE);
  setvbuf(stdin, NULL, _IONBF, 0);
  #endif

}

/**
  * @brief  Main task that initializes RTOS objects and starts BMS threads.
  * @param  UNUSED pointer passed by thread creation
  * @retval None
  */
void mainTask(void *argument)
{
  // RTOS Objects initialization
  bmsMutexHandle = xSemaphoreCreateMutex();
  ioMutexHandle = xSemaphoreCreateMutex();

  // Create event flags for charging mode
  charging_evt_id = xEventGroupCreate();

  dischargingJustEnabled = DISCHARGE_JUST_ENABLED;
  xEventGroupSetBits(charging_evt_id, EVENT_FLAG_CHARGING_DISABLE);
  faultLatchTimerHandle = xTimerCreate("faultLatchTimer",
                                       pdMS_TO_TICKS(500),
                                       pdFALSE,
                                       NULL,
                                       bmsFaultHandle);

  xTaskCreate(bmsDataCanTxTask,
              "bmsDataCanTxTask",
              configMINIMAL_STACK_SIZE,
              NULL,
              PRIORITY_LOW,
              NULL);
  xTaskCreate(dischargingTask,
              "dischargingTask",
              (configMINIMAL_STACK_SIZE * 2),
              NULL,
              PRIORITY_LOW,
              NULL);
  xTaskCreate(chargingTask,
              "chargingTask",
              (configMINIMAL_STACK_SIZE * 2),
              NULL,
              PRIORITY_LOW,
              NULL);
  #ifdef TESTBENCH
    testbench_init();
  #endif
  for (;;)
  {
    #ifndef TESTBENCH
    HAL_GPIO_TogglePin(Heartbeat_GPIO_Port, Heartbeat_Pin);
    #endif
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

/**
  * @brief  The application entry point; initializes hardware and starts the FreeRTOS scheduler.
  * @param None
  * @retval int
  */
int main(void)
{
  #ifndef TESTBENCH
  HAL_Init();
  SystemClock_Config();
  #endif
  peripheralsInit();
  #ifdef TESTBENCH
  for (int i = 0; i < ADBMS_6830_IC_NUM; i++) {
    characteristic_6830[i].ic_data = &ADBMS_6830_IC[i];
  }
  for (int i = 0; i < ADBMS_2950_IC_NUM; i++) {
    characteristic_2950[i].ic_data = &ADBMS_2950_IC[i];
  }
  #endif
  xTaskCreate(mainTask, "mainTask", configMINIMAL_STACK_SIZE, NULL, PRIORITY_IDLE, NULL);
  vTaskStartScheduler();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

/**
  * @brief System Clock Configuration
  * @retval None
  */
#ifndef TESTBENCH
  void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 6;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}
#endif

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
