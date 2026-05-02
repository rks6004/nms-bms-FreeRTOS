//standard includes
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "event_groups.h"

//Analog-Devices-related includes
#include "adBms6830Driver.h"
#include "adBms6830Data.h"

#include "adBms6830_TESTBENCH.h"
#include "bms_test.h"

extern SemaphoreHandle_t ioMutexHandle;
extern EventGroupHandle_t charging_evt_id;
extern emulated_adbms_6830 characteristic_6830[ADBMS_6830_IC_NUM];

static inline float cell_voltage_adjuster(uint32_t millis) {return ((0.05 * (float)(millis)) / (1000 * 60));} //cells lose 0.05V per minute in normal operation
static inline bool probability_check(int percent) {return rand() < ((percent * RAND_MAX) / 100);}
 
//test values parsing average for each segment cell
void adBms6830_read_avgcell_voltages_testbench(uint8_t tIC, cell_asic_6830 *ic, GRP group) {
    //printf("polling 6830 voltage.\n");
    uint32_t data_timer = pdTICKS_TO_MS(xTaskGetTickCount()) % TEST_STREAM_MAX_LENGTH_MS; //datastream will loop if not terminated when EOF of datastream reached
    uint32_t data_index = (data_timer / TEST_STREAM_TIMING_RESOLUTION);
    for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++) 
    {
        int32_t test_voltage = characteristic_6830->voltage_data->values[data_index];
        //printf("Set voltage: %d\n", test_voltage);
        if (test_voltage > INT16_MAX) {
            test_voltage = INT16_MAX; //ceiling reading for 6830 register datatype
        }
        int16_t altered_voltages[CELL];   
        for (int cell = 0; cell < CELL; cell++) {
            int16_t indiv_test_voltage = test_voltage;
            indiv_test_voltage -= (rand() % 100); //fuzzes cell voltages mildly
            //printf("Set voltage: %d\n", indiv_test_voltage);
            altered_voltages[cell] = indiv_test_voltage;
        }
        if (group != ALL_GRP && group != F) {
            memcpy(&(ic[curr_ic].acell.ac_codes[(group - 1) * AVG_GROUP_SIZE]), altered_voltages, (size_t)(AVG_GROUP_SIZE * sizeof(int16_t)));
        }
        else if (group == F) {
            memcpy(&(ic[curr_ic].acell.ac_codes[(group - 1) * AVG_GROUP_SIZE]), altered_voltages, (size_t)(sizeof(int16_t))); //only one register in group here
        }
        else { //ALLGRP
            memcpy(&(ic[curr_ic].acell.ac_codes), altered_voltages, (size_t)(CELL * sizeof(int16_t)));
        }
        switch (characteristic_6830[curr_ic].signal_behavior)
        {
        case PEC_NORMAL:
            //low probability of failing interference
            ic[curr_ic].cccrc.acell_pec = (uint8_t)(probability_check(PEC_NORMAL_PROB));
            break;
        case PEC_SLIGHT_INTERFERENCE:
            //slight probability of failing interference
            ic[curr_ic].cccrc.acell_pec = (uint8_t)(probability_check(PEC_SLIGHT_PROB));
            break;
        case PEC_HEAVY_INTERFERENCE:
            //statistically significant probability of failing interference
            ic[curr_ic].cccrc.acell_pec = (uint8_t)(probability_check(PEC_HEAVY_PROB));
            break;
        default:
            xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
            printf("Invalid PEC strength setting in 6830 emulation, exiting application: %d\n", characteristic_6830[curr_ic].signal_behavior);
            xSemaphoreGive(ioMutexHandle);
            exit(EXIT_FAILURE);
            break;
        }    
    }
}

//test values parsing thermistor inputs from auxillary GPIOs for segment area temps 
void adBms6830_read_aux_voltages_testbench(uint8_t tIC, cell_asic_6830 *ic, GRP group) {
    uint32_t data_timer = pdTICKS_TO_MS(xTaskGetTickCount()) % TEST_STREAM_MAX_LENGTH_MS; //datastream will loop if not terminated when EOF of datastream reached
    uint32_t data_index = (data_timer / TEST_STREAM_TIMING_RESOLUTION);
    for(uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++) 
    {
        int32_t test_temp = characteristic_6830->temp_data->values[data_index];
        if (test_temp > INT16_MAX) {
            test_temp = INT16_MAX; //ceiling reading for 6830 register datatype
        }
        int16_t altered_voltages[CELL];   
        for (int cell = 0; cell < CELL; cell++) {
            int16_t indiv_test_temp = test_temp;
            indiv_test_temp -= (rand() % 100); //fuzzes cell voltages mildly
            altered_voltages[cell] = indiv_test_temp;
        }
        //A, B, C, D
        if (group != ALL_GRP && group < E) {
            memcpy(&(ic[curr_ic].aux.a_codes[(group - 1) * AUX_GROUP_SIZE]), altered_voltages, (size_t)(AUX_GROUP_SIZE * sizeof(int16_t)));
        }
        else if (group == ALL_GRP){ //ALLGRP
            memcpy(&(ic[curr_ic].aux.a_codes), altered_voltages, (size_t)(AUX * sizeof(int16_t)));
        }
        else { //E,F not valid groups
            xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
            printf("Invalid group access in read_aux: %d\n", group);
            xSemaphoreGive(ioMutexHandle);
            exit(EXIT_FAILURE);
        }
        switch (characteristic_6830[curr_ic].signal_behavior)
        {
        case PEC_NORMAL:
            //low probability of failing interference
            ic[curr_ic].cccrc.aux_pec = (uint8_t)(probability_check(PEC_NORMAL_PROB));
            break;
        case PEC_SLIGHT_INTERFERENCE:
            //slight probability of failing interference
            ic[curr_ic].cccrc.aux_pec = (uint8_t)(probability_check(PEC_SLIGHT_PROB));
            break;
        case PEC_HEAVY_INTERFERENCE:
            //statistically significant probability of failing interference
            ic[curr_ic].cccrc.aux_pec = (uint8_t)(probability_check(PEC_HEAVY_PROB));
            break;
        default:
            xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
            printf("Invalid PEC strength setting in 6830 emulation, exiting application: %d\n", characteristic_6830[curr_ic].signal_behavior);
            xSemaphoreGive(ioMutexHandle);
            exit(EXIT_FAILURE);
            break;
        }

    }
}



void adBms6830_populate_cell_temps_testbench(uint8_t tIC, cell_asic_6830* ic) {
    //printf("polling 6830 temps.\n");
    uint32_t data_timer = pdTICKS_TO_MS(xTaskGetTickCount()) % TEST_STREAM_MAX_LENGTH_MS; //datastream will loop if not terminated when EOF of datastream reached
    uint32_t data_index = (data_timer / TEST_STREAM_TIMING_RESOLUTION);
    for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++) 
    {
        int32_t recorded_temp = characteristic_6830->temp_data->values[data_index]; //temp stored in mC
        for (uint8_t temp = 0; temp < AUX-2; temp++) {
            float temp_adjusted = ((float)(recorded_temp - (rand() % 100))) / 1000.0f;
            ic[curr_ic].cell_temperatures.cell_temps[temp] = temp_adjusted;
            //printf("Set temp: %f\n", temp_adjusted);
        }
    }    
}
