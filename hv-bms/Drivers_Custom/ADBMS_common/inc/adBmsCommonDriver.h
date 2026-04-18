#ifndef ADBMS_COMMON_DRIVER
#define ADBMS_COMMON_DRIVER

/**
 * @file adBmsCommonDriver.h
 * @author Jacopo Di Prato (jfd5878@psu.edu)
 * @brief This file contains all functions that combine communication to both 2950 and 6830 
 * devices. This is needed for commands that are compatible with both devices. Take a look at
 * the Network Layer section of either datasheets for better understaiding (p. 22 of ADBMS2950
 * Datasheet, p. 56 of ADBMS6830 Datasheet)
 * @version 0.0
 * @date 2025-01-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "adBms2950Driver.h"
#include "adBms6830Driver.h"

/**
 * @brief Function to write commands to both ADBMS2950 and ADBMS6839 chips located on the same BMS
 * chain. This method assumes that the ADBMS2950 chips are placed at the beginning of the chain, 
 * while the ADBMS6830 chips are placed following the ADBMS2950 chips. Not following this order in
 * the hardware will result in faulty communication.
 * 
 * @param t6830IC Number of ADBMS6830 ICs in the chain
 * @param t2950IC Number of ADBMS2950 ICs in the chain
 * @param ic_6830 Pointer to ADBMS6830 objects (containing all current register values)
 * @param ic_2950 Pointer to ADBMS2950 objects (containing all current register values)
 * @param cmd_arg 2-byte command code
 * @param type Command type
 * @param group Group of the command
 */
void adBmsCommonWriteData(uint8_t t6830IC, uint8_t t2950IC, cell_asic_6830 *ic_6830, cell_asic_2950 *ic_2950, uint8_t cmd_arg[2], TYPE type, GRP group);

/**
 * @brief Function that writes configuration registers A (CFGA) and B (CFGB) to all ICs, ADBMS6830
 * and ADBMS2950, in the BMS chain.
 * 
 * @param t6830IC Number of ADBMS6830 ICs in the chain
 * @param t2950IC Number of ADBMS2950 ICs in the chain
 * @param ic_6830 Pointer to ADBMS6830 objects (containing all current register values)
 * @param ic_2950 Pointer to ADBMS2950 objects (containing all current register values)
 */
void adBmsCommonWriteConfig(uint8_t t6830IC, uint8_t t2950IC, cell_asic_6830 *ic_6830, cell_asic_2950 *ic_2950);

#endif

