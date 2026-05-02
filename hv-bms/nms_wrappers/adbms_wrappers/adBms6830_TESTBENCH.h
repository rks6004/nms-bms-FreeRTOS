#ifndef ADBMS6830_TESTBENCH_H
#define ADBMS6830_TESTBENCH_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

#include "bms_test.h"
#include "main.h"

//Analog-Devices-related includes
#include "adBms6830Driver.h"
#include "adBms6830Data.h"

//DEFINES

#define OPTIMAL_CELL_VOLTAGE MAX_CELL_VOLTAGE - 0.05 //V
#define OPTIMAL_CELL_TEMP 25 //deg C, picking ~ambient temp


/* ============================================================================
 * VOLTAGE CONVERSION MACROS
 * ============================================================================
 * Reverse of get_voltage(): adc_code = (voltage / 0.000150) - 10000
 */

/**
 * @brief Convert voltage (float) to int16_t ADC code
 * @param voltage_v Voltage in volts (e.g., 4.0 for 4.0V)
 * @return int16_t ADC code to be stored in acv_.ac_codes[]
 * 
 * Example:
 *   int16_t adc = VOLTAGE_TO_ADC_CODE(4.15);  // Returns 17667
 *   ic[0].acell.ac_codes[0] = adc;
 */
#define VOLTAGE_TO_ADC_CODE(voltage_v) \
  ((int16_t)(((voltage_v) / 0.000150) - 10000))

/**
 * @brief Convert voltage (float) to unsigned 16-bit for byte extraction
 * @param voltage_v Voltage in volts
 * @return uint16_t ADC code (unsigned representation)
 */
#define VOLTAGE_TO_ADC_UNSIGNED(voltage_v) \
  ((uint16_t)VOLTAGE_TO_ADC_CODE(voltage_v))

/**
 * @brief Extract LSB from voltage
 * @param voltage_v Voltage in volts
 * @return uint8_t Least significant byte (data[0] in SPI register)
 */
#define VOLTAGE_TO_LSB(voltage_v) \
  ((uint8_t)(VOLTAGE_TO_ADC_UNSIGNED(voltage_v) & 0xFF))

/**
 * @brief Extract MSB from voltage
 * @param voltage_v Voltage in volts
 * @return uint8_t Most significant byte (data[1] in SPI register)
 */
#define VOLTAGE_TO_MSB(voltage_v) \
  ((uint8_t)((VOLTAGE_TO_ADC_UNSIGNED(voltage_v) >> 8) & 0xFF))


/*
====================================
SAMPLE SEGMENT STRUCTS FOR TESTING
====================================
*/

// These const arrays are now defined in adBms6830_TESTBENCH_globals.c
// to avoid multiple definition errors when this header is included
// in multiple translation units.
extern const int16_t optimal_cell_voltage_adc;
extern const acv_ optimal_voltages;
extern const cell_temps_ optimal_temps;
extern const int16_t optimal_cell_temp_adc;
extern const ax_ optimal_temps_adcs;

void adBms6830_read_avgcell_voltages_testbench(uint8_t tIC, cell_asic_6830 *ic, GRP group);

void adBms6830_read_aux_voltages_testbench(uint8_t tIC, cell_asic_6830 *ic, GRP group);

void adBms6830_populate_cell_temps_testbench(uint8_t tIC, cell_asic_6830* ic);


#endif //header guard