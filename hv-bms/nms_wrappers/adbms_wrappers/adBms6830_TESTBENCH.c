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
 
void adBms6830_read_avgcell_voltages_testbench(uint8_t tIC, cell_asic_6830 *ic, GRP group) {
    uint32_t data_timer = pdTICKS_TO_MS(xTaskGetTickCount()) % TEST_STREAM_MAX_LENGTH_MS; //datastream will loop if not terminated when EOF of datastream reached
    uint32_t data_index = (data_timer / TEST_STREAM_TIMING_RESOLUTION);
    for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++) 
    {
        int32_t test_voltage = characteristic_6830->voltage_data->values[data_index];
        if (test_voltage > INT16_MAX) {
            test_voltage = INT16_MAX; //ceiling reading for 6830 register datatype
        }
        
        int16_t altered_voltages[CELL];   
        memcpy(altered_voltages, &optimal_voltages.ac_codes, CELL * sizeof(int16_t));
        for (int cell = 0; cell < CELL; cell++) {
            altered_voltages[cell] -= (rand() % 100); //fuzzes cell voltages mildly
            #ifndef CHARGING_TEST
            altered_voltages[cell] -= VOLTAGE_TO_ADC_CODE(cell_voltage_adjuster(current_time_ms));
            #else
            
            #endif
        }
        switch (characteristic_6830[curr_ic].volt_behavior)
        {
        case VOLT_NORMAL:
            //only need to fuzz voltage values slightly for normal voltage, no further action needed   
            break;
        case VOLT_OV:
            //BMS will continously receive some cells at MAX_CELL_VOLTAGE
            int ov_cells = rand() % CELL;
            for (int rand_cell = 0; rand_cell < ov_cells; rand_cell++) {
                altered_voltages[rand() % CELL] = VOLTAGE_TO_ADC_CODE(MAX_CELL_VOLTAGE);
            }
            break;
        case VOLT_UV:
            //BMS will continously receive some cells at MIN_CELL_VOLTAGE
            int uv_cells = rand() % CELL;
            for (int rand_cell = 0; rand_cell < uv_cells; rand_cell++) {
                altered_voltages[rand() % CELL] = VOLTAGE_TO_ADC_CODE(MIN_CELL_VOLTAGE);
            }
            break;
        default:
            xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
            printf("Invalid voltage emulation state: %d\n", characteristic_6830[curr_ic].volt_behavior);
            xSemaphoreGive(ioMutexHandle);
            exit(EXIT_FAILURE);
            break;
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
            printf("Invalid PEC strength setting in emulation: %d\n", characteristic_6830[curr_ic].signal_behavior);
            xSemaphoreGive(ioMutexHandle);
            exit(EXIT_FAILURE);
            break;
        }    
    }
}

void adBms6830_read_aux_voltages_testbench(uint8_t tIC, cell_asic_6830 *ic, GRP group) {
    for(uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++) 
    {
        int16_t altered_voltages[AUX];   
        memcpy(altered_voltages, optimal_temps_adcs.a_codes, AUX * sizeof(int16_t));
        for (int temp_adc = 0; temp_adc < AUX; temp_adc++) {
            altered_voltages[temp_adc] -= (rand() % 100); //fluctates temp voltages by ~1 degC max
        }
        switch (characteristic_6830[curr_ic].temp_behavior)
        {
        case TEMP_NORMAL:
            //only need to fuzz voltage values slightly for normal temps, no further action needed   
            break;
        case TEMP_OT:
            //BMS will continously receive some cells at MAX_CELL_TEMP
            int ot_cells = rand() % AUX;
            for (int rand_cell = 0; rand_cell < ot_cells; rand_cell++) {
                altered_voltages[rand() % AUX] = (int16_t)0xEB26;
            }
            break;
        case TEMP_UT:
            //BMS will continously receive some cells at MIN_CELL_TEMP
            int ut_cells = rand() % AUX;
            for (int rand_cell = 0; rand_cell < ut_cells; rand_cell++) {
                altered_voltages[rand() % AUX] = (int16_t)0x1DE0;
            }
            break;
        default:
            xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
            printf("Invalid temperature emulation state: %d\n", characteristic_6830[curr_ic].temp_behavior);
            xSemaphoreGive(ioMutexHandle);
            exit(EXIT_FAILURE);
            break;
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
            printf("Invalid PEC strength setting in emulation: %d\n", characteristic_6830[curr_ic].signal_behavior);
            xSemaphoreGive(ioMutexHandle);
            exit(EXIT_FAILURE);
            break;
        }

    }
}




