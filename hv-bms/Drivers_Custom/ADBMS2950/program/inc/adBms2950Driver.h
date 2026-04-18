/**
********************************************************************************
*
* @file:    adi_2950.h
*
* @brief:   This file contains the test cases functions.
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
/*! \addtogroup Main
*  @{
*/

/*! \addtogroup Test_Cases
*  @{
*/

#ifndef __ADI2950_H
#define __ADI2950_H

#include "adBms2950GenericType.h"
#include "adBms2950PrintResult.h"

#define I1CNT_DEFAULT (uint16_t)0

/**
 * @brief Header for functional test cases
 * 
 * @details This function has the implementation of header.
 * Steps:
 * 1.  Send CS toggle to wakeup the IC
 * 2.  Send SRST command with a delay to ensure the soft reset             
 * 3.  Reset command counter.
 * 4.  Read CFGA until REFUP bit is high
 * 5.  Send RDSID to read SID
 * 6.  Send RDSTAT to read REV ID 
 * 7.  Send CLRFLAG to clear flag register
 * 8.  Send RDFLAG to read flag register
 * 
 * @param tIC Total number of ICs
 * @param ic cell_asic_2950 stucture pointer
 */
void adBms2950_Functional_header(uint8_t tIC, cell_asic_2950 *ic);

/**
 * @brief Reset command counter
 * 
 * @param tIC Total number of ICs
 * @param ic cell_asic_2950 stucture pointer
 */
void adBms2950_reset_cmd_count(uint8_t tIC, cell_asic_2950 *ic);

/**
 * @brief Helper function to configure clrflag register data
 * 
 * @param cic index of the ADBMS2950 IC in the BMS chain that needs flags to be cleared
 * @param tIC Total number of ICs
 * @param ic cell_asic_2950 stucture pointer
 */
void adBms2950_Configure_CLRflag_Data(uint8_t cic, uint8_t tIC, cell_asic_2950 *ic);

/**
 * @brief Set configuration register A (CFGA). Refer to the data sheet
 *        Set configuration register B (CFGB). Refer to the data sheet
 * 
 * @param tIC Total number of ICs
 * @param ic cell_asic_2950 stucture pointer
 */
void adBms2950_init_config(uint8_t tIC, cell_asic_2950 *ic);

/**
 * @brief Read Battery Voltages and Current Registers Accumulators
 * 
 * @param tIC Total number of ICs
 * @param ic cell_asic_2950 stucture point
 */
void adBms2950_read_acc_ivbat(uint8_t tIC, cell_asic_2950 *ic);

#endif /* __ADI2950_H */

/** @}*/
/** @}*/