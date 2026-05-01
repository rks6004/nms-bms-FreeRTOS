/**
 * @file adBms6830_TESTBENCH_globals.c
 * @brief Global constant arrays for ADBMS6830 testbench
 * 
 * This file contains the actual definitions of const arrays that are used
 * in testbench mode. These are defined here (not in header) to avoid
 * multiple definition errors when the header is included in multiple .c files.
 */

#include "adBms6830_TESTBENCH.h"

const int16_t optimal_cell_voltage_adc = VOLTAGE_TO_ADC_CODE(OPTIMAL_CELL_VOLTAGE);

const acv_ optimal_voltages = {
    .ac_codes = {
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //A
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //B
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //C
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //D
        optimal_cell_voltage_adc, optimal_cell_voltage_adc, optimal_cell_voltage_adc, //E
        optimal_cell_voltage_adc                                                      //F
    }
};

const cell_temps_ optimal_temps = {
    .cell_temps = {
        (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, //A
        (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, //B
        (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, (float)OPTIMAL_CELL_TEMP, //C
        (float)OPTIMAL_CELL_TEMP                                                      //D 
    }
};

const int16_t optimal_cell_temp_adc = 0; //derived from OPTIMAL_CELL_TEMP, but calculated seperately due to inability to use math functions in precompilation

const ax_ optimal_temps_adcs = {
    .a_codes = {
        optimal_cell_temp_adc, optimal_cell_temp_adc, optimal_cell_temp_adc,    //A
        optimal_cell_temp_adc, optimal_cell_temp_adc, optimal_cell_temp_adc,    //B
        optimal_cell_temp_adc, optimal_cell_temp_adc, optimal_cell_temp_adc,    //C
        optimal_cell_temp_adc, optimal_cell_temp_adc, optimal_cell_temp_adc     //D
    }
};
