/**
********************************************************************************
*
* @file:    adBms2950GenericType.h
*
* @brief:   This file contains utility functions.
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
/*! \addtogroup BMS_Driver
*  @{
*/

/*! \addtogroup Driver_Utility
*  @{
*/

#ifndef __ADI_BMS_UTILITY_H
#define __ADI_BMS_UTILITY_H

/*============= I N C L U D E S =============*/
/*============== D E F I N E S ===============*/
/*============= E X T E R N A L S ============*/
/*============= E N U M E R A T O R S ============*/

#include "common.h"
#include "adBms2950Data.h"
#include "mcuWrapper.h"
#include "adBmsUtility.h"

void spiSendCmd2950(uint8_t tIC, cell_asic_2950 *ic, uint8_t tx_cmd[2]);
void adBms2950ReadData(uint8_t tIC, cell_asic_2950 *ic, uint8_t cmd_arg[2], DATA_TYPE type, REG_GRP group);
void adBms2950WriteData(uint8_t tIC, cell_asic_2950 *ic, uint8_t cmd_arg[2], DATA_TYPE type, REG_GRP group);
uint32_t adBms2950PollAdc(uint8_t tIC, cell_asic_2950 *ic, uint8_t tx_cmd[2]);
void adBms2950_Adi1(uint8_t tIC, cell_asic_2950 *ic, RD rd, OPT opt, cmd_description *cmd_des);
void adBms2950_Adi2(uint8_t tIC, cell_asic_2950 *ic, OPT opt, cmd_description *cmd_des);
void adBms2950_Adv(uint8_t tIC, cell_asic_2950 *ic, OW ow, VCH vch);
void adBms2950_Stcomm(uint8_t tIC, cell_asic_2950 *ic);
uint8_t adBms2950_CCErrorCheck(uint8_t ic_cc, uint8_t cal_cc);

#endif /* __UTILITY_H */
/** @}*/
/** @}*/