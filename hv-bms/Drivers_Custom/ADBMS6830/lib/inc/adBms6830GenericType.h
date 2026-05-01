/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    adBms6830GenericType.h
* @brief:   Generic Type function header file
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/** @addtogroup BMS_DRIVER
*  @{
*
*/

/** @addtogroup GENERIC_TYPE GENERIC TYPE
*  @{
*
*/
#ifndef __adBmsGenericType_H
#define __adBmsGenericType_H

#ifndef TESTBENCH
#include "mcuWrapper.h"
#endif
#include "adBms6830ParseCreate.h"
#include "adBmsUtility.h"

#ifndef TESTBENCH
void spiSendCmd6830(uint8_t tx_cmd[2]);
#endif
void adBms6830ReadData(uint8_t tIC, cell_asic_6830 *ic, uint8_t cmd_arg[2], TYPE type, GRP group);

#ifndef TESTBENCH
void adBms6830WriteData(uint8_t tIC, cell_asic_6830 *ic, uint8_t cmd_arg[2], TYPE type, GRP group);
uint32_t adBmsPollAdc(uint8_t tx_cmd[2]);
#endif

#ifndef TESTBENCH
void adBms6830_Adcv
(
  RD rd,
  CONT cont,
  DCP dcp,
  RSTF rstf,
  OW_C_S owcs
);

void adBms6830_Adsv
(
  CONT cont,
  DCP dcp,
  OW_C_S owcs
);

void adBms6830_Adax
(
OW_AUX owaux, 							
PUP pup,
CH ch
);

void adBms6830_Adax2
(
  CH ch
);
#endif

/** @}*/
/** @}*/

#endif
/** @}*/