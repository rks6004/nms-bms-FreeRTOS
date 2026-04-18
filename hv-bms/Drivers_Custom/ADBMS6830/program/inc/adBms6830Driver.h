/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    adBms_Application.h
* @brief:   Bms application header file
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! @addtogroup APPLICATION
*  @{
*
*/

/*! @addtogroup APPLICATION
*  @{
*
*/

#ifndef __ADBMS6830_DRIVER_H
#define __ADBMS6830_DRIVER_H

#include "adBms6830GenericType.h"
#include "adBmsCmdList.h"

/**
 * @brief Initialize the ADBMS6830 ConfigA
 * and ConfigB for standard operation, meaning that the 
 * car is running normally, so the battery is discharging. 
 * We currently do not support balancing cells during 
 * discharge.
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_init_config(uint8_t tIC, cell_asic_6830 *ic);

/**
 * @brief Initialize conversions for cell
 * voltage ADCs so that we can perform continuous read during
 * normal operation of the battery (discharge).
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_init_measurements(uint8_t tIC, cell_asic_6830 *ic);

/**
 * @brief Initialize the ADBMS6830 ConfigA
 * and ConfigB for charging operation. While the battery 
 * is charging, the BMS should balance the cells. 
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_init_charging_config(uint8_t tIC, cell_asic_6830 *ic);

/**
 * @brief Initialize conversions for cell voltage ADC taking
 * into consideration charging state of the battery, 
 * so cell balancing. The conversions configuration is
 * slightly different than that of regular operation. 
 * Check datasheet for more information
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_init_charging_measurements(uint8_t tIC, cell_asic_6830 *ic);

/**
 * @brief Read average cell voltage registers during regular
 * battery operation (discharge). Simply read RDAC* registers 
 * for all ADBMS6830 in the chain.
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_read_avgcell_voltages(uint8_t tIC, cell_asic_6830 *ic);

/**
 * @brief Initialize auxiliary voltage registers conversions.
 * This is used to read ADC values of the thermistors connected
 * to the cells to monitor cell temperatures.
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_start_aux_voltage_measurment(uint8_t tIC, cell_asic_6830 *ic);

/**
 * @brief Read auxiliary voltage registers.
 * This is used to read ADC values of the thermistors connected
 * to the cells to monitor cell temperatures.
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_read_aux_voltages(uint8_t tIC, cell_asic_6830 *ic);

/**
 * @brief Read average cell voltage registers charging operation. 
 * This involves sending ADSV command to initialize S-ADC conversions
 * to perform reduntant measurements and pause PWM balancing. Then 
 * read RDAC* registers for all ADBMS6830 in the chain, and finally 
 * initialize S-ADC conversions for single shot so that PWM balancing
 * is resumed after last conversion (consult the datasheet for more details).
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_charging_voltage_measurement(uint8_t tIC, cell_asic_6830* ic);

/**
 * @brief This function implements the logic to determine which cells 
 * to balance at each measurement cycle. In short, find the minimum cell
 * voltage in each segment, balance all the cells that have a difference 
 * greater than 10 mV from the minimum voltage.
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param t2950IC Number of ADBMS2950 in the chain to ignore for balancing logic
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_balance_cells(uint8_t tIC,  uint8_t t2950IC, cell_asic_6830* ic);

/**
 * @brief This function populates the struct that holds cell temperatures 
 * in degrees celcius. This is achieved by converting the AUX ADC reading 
 * into a voltage and then into a temperature using the thermistor's ratings.
 * 
 * @param tIC Number of ADBMS6830 in the chain
 * @param ic Pointer to first IC local register copy. 
 * The register structure of each IC is replicated 
 * using an array of cell_asic_6830 structs.
 */
void adBms6830_populate_cell_temps(uint8_t tIC, cell_asic_6830* ic);

#endif
/** @}*/
/** @}*/