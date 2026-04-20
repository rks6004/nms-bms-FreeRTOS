#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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

//many of these will be derived from command-line arguments
#ifndef VOLTAGE_TEST
    #define VOLTAGE_TEST VOLT_NORMAL
#endif

#ifndef TEMP_TEST
    #define TEMP_TEST TEMP_NORMAL
#endif

#ifndef CURRENT_TEST
    #define CURRENT_TEST CURR_NORMAL
#endif

#ifndef PEC_TEST
    #define PEC_TEST PEC_NORMAL
#endif

#define OPTIMAL_CELL_VOLTAGE 4.15 //V
#define OPTIMAL_CELL_TEMP 25 //deg C, picking ~ambient temp
#define OPTIMAL_ERROR_COUNT 0


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

const int16_t optimal_cell_voltage_adc = VOLTAGE_TO_ADC_CODE(OPTIMAL_CELL_VOLTAGE);
//4.15 V across all cells in segment when translated
const extern acv_ optimal_voltages = {
    .ac_codes = {
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //A
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //B
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //C
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //D
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //E
        optimal_cell_voltage_adc                                                      //F
    }
};

const extern cell_temps_ optimal_temps = {
    .cell_temps = {
        (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, //A
        (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, //B
        (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, //C
        (float)OPTIMAL_CELL_TEMP                                                      //D 
    }
};

const int16_t optimal_cell_temp_adc = 0; //derived from OPTIMAL_CELL_TEMP, but calculated seperately due to inability to use math functions in precompilation
const extern ax_ optimal_temps_adcs = {
    .a_codes = {
        optimal_cell_temp_adc, optimal_cell_temp_adc, optimal_cell_temp_adc,    //A
        optimal_cell_temp_adc, optimal_cell_temp_adc, optimal_cell_temp_adc,    //B
        optimal_cell_temp_adc, optimal_cell_temp_adc, optimal_cell_temp_adc,    //C
        optimal_cell_temp_adc, optimal_cell_temp_adc, optimal_cell_temp_adc     //D
    }
};

void adBms6830_read_avgcell_voltages_testbench(uint8_t tIC, cell_asic_6830 *ic, GRP group);

void adBms6830_read_aux_voltages_testbench(uint8_t tIC, cell_asic_6830 *ic, GRP group);




