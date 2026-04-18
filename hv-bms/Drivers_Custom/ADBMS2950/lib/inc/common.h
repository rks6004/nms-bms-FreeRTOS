/**
********************************************************************************
*
* @file:    common.h
*
* @brief:   Standard common header files.
*
* @details:
*
*******************************************************************************
Copyright(c) 2020 Analog Devices, Inc. All Rights Reserved. This software is
proprietary & confidential to Analog Devices, Inc. and its licensors. By using
this software you agree to the terms of the associated Analog Devices License
Agreement.
*******************************************************************************
*/

/*! \addtogroup Common
*  @{
*/

#ifndef __COMMON_H_
#define __COMMON_H_

/*============= I N C L U D E S =============*/
/*============== D E F I N E S ===============*/
/*============= E X T E R N A L S ============*/
/*============= E N U M E R A T O R S ============*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

//#define MBED /* Switch between IAR Workbench or Mbed Studio.*/
//#define FAST_SPI /* Switch between FAST_SPI or HAL_SPI.*/
//#define PYBMS /* Switch between Local Run and PyBMS run.*/
#define POLLING /* Switch between polling and delay method for conversion.*/
//#define UNTRIMMED /* Switch between Untrimmed and Trimmed part validator checks.*/
//#define DEBUG /* Enable and disable print statements for debug. This should be disabled by default*/
//#define RAW_DEBUG /* Enable and disable print statements for all driver functions debug. This should be disabled by default*/
#define SPI_DAC_OFFSET 25 /* This offset is introduced when LDAC is set and then again SPI command is set to change the voltage*/
//#define REVC 
#define SRST_delay 2 //SRST delay is roughly 1.44mS
#define REFUP_delay 5 /* For the reference to go up , typically 3.5ms*/

#define LDAC_UPDATE_RATE 10 /* in micro seconds*/ 
#define RDV_READ_TIME 74 /* Time taken by single read command */
#define ADV_CONVERSION_TIME 270  /* Conversion time of single channel in VADC */
#define REVB_OC_CLRABC_WrkArnd 
#define INCREASING_VTG_SWEEP /* To switch between increasing or decreasing voltage sweep  for Fun44 */
//#define VOLTAGE_VALUE_RETENTION /* Enable this to retain the values of voltage registers i.e. no clear commands */
// #define ISOSPI_WAKEUP  /* Enable this to provide wakeup before every write and read command */
#define RegulatorStartupTime_us 600  /* BMS IC regulator startup time in micro seconds */
#define CalibrationTime_ms 130  /* BMS IC calibration time in milli seconds */
// #define WAKEUP_us_DELAY 250  /* BMS IC wakeup delay in micro seconds */
#define WAKEUP_us_DELAY 2  /* BMS IC wakeup delay in micro seconds */
#define WAKEUP_us /* Enable this to provide wakeup in micro seconds , can be used in case of only 1 board in daisy chain  */
//#define WAKEUP_DELAY 2/* BMS ic wakeup delay  ms*/
#define OC_delay_8cycle_us 516     /*delay to see the effect in OC pins and OC registers after each OCEN toggle and CLRFLAG to clear the OC related flags in flag register (8*64.5us)*/
#define OC_InitialDelayAfterOCEN_us 150     /*delay for OCRs to get updated for the first time when OCEN =1; it is more than 2 cycles*/
#define OC_delay_10cycles_us 640
#define spi_DAC_ReadbackTime_ms 20     /*spi DAC readback time in ms*/
#define Polling_Delay_ms 5
#define ADI1_delay_ms 9     /*Startup delay for ADI1 in ms*/
#define spiDAC_VoltageSetDelay_us 10     /*Voltage set delay in spi DAC in us*/
#define OC_Clrflag_delay 500

#define OCVREG /* Switch between VREG and 3.3V as OC pull-up. Comment to enable 3.3V PU*/
#define OC_POL_HIGH /* Switch between HIGH or LOW output from OC alert pins. Set this to toggle OCAX and OCBX polarity*/
//#define GPO3V3 /* Switch between 3.3V and VDD as GPO pull-up. Comment to enable VDD PU */

// Default Voltages
#define VDD_Default_Voltage  11.00 /* VDD Default input voltage */
#define VREG_Default_Voltage 5.00  /* VREG Default input voltage */

#endif /*__COMMON_H_ */

/** @}*/