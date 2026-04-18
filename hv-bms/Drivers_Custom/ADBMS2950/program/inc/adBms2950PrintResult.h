/**
********************************************************************************
*
* @file:    adBms2950PrintResult.h
*
* @brief:   This file contains the print result.
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

/*! \addtogroup Print_Result
*  @{
*/

#ifndef __RESULT_H
#define __RESULT_H

#include "adBms2950Data.h"
#include "common.h"

void printWriteConfig2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type, REG_GRP grp);
void printReadConfig2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type, REG_GRP grp);
void printDeviceSID2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type);
void printWriteCommData2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type);
void printReadCommData2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type);
void printCr2950(uint8_t tIC, cell_asic_2950 *IC);
void printVoltages2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type);
void printVbat2950(uint8_t tIC, cell_asic_2950 *IC);
void printIvbat2950(uint8_t tIC, cell_asic_2950 *IC);
void printAccVbat2950(uint8_t tIC, cell_asic_2950 *IC);
void printAvgAccIVbat2950(uint8_t tIC, cell_asic_2950 *IC);
float getPackVoltage(uint8_t tIC, cell_asic_2950 *IC);
float getPackCurrent(uint8_t tIC, cell_asic_2950 *IC);
void printAccIVbat2950(uint8_t tIC, cell_asic_2950 *IC);
void printAccCr2950(uint8_t tIC, cell_asic_2950 *IC);
void printOc2950(uint8_t tIC, cell_asic_2950 *IC);
void printRdallv2950(uint8_t tIC, cell_asic_2950 *IC); 
void printRdallr2950(uint8_t tIC, cell_asic_2950 *IC);
void printRdallx2950(uint8_t tIC, cell_asic_2950 *IC);
void printGpV12950(uint8_t tIC, cell_asic_2950 *IC);
void printStatus2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type, REG_GRP grp);
void printFlag2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type);
void printAux2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type, REG_GRP grp);
void printMenu2950(void);
void printPollAdcConvTime2950(int count);
void printResultCount2950(int count);
void readUserInupt2950(int *user_command);
float getVoltage2950(int data);
float getCurrent2950(uint32_t data);
float getAccCurrent2950(uint32_t data);
float getAccVbat2950(uint32_t data);
float getOverCurrent2950(uint8_t data);
float getAvgAccVbat2950(uint32_t data, uint32_t accn);
float getAvgAccCurrent2950(uint32_t data, uint32_t accn);

#endif /* __RESULT_H */
/** @}*/
/** @}*/