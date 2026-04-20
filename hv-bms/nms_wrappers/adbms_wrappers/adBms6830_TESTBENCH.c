//standard includes
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include "bms_test.h"

//Analog-Devices-related includes
#include "adBms6830Driver.h"
#include "adBms6830Data.h"

#include "adBms6830_TESTBENCH.h"

extern VOLTAGE_STATE voltage_testing;
extern TEMP_STATE temp_testing;
extern CURRENT_STATE current_testing;
extern PEC_STATE pec_testing; 

void adBms6830_read_avgcell_voltages_testbench(uint8_t tIC, cell_asic_6830 *ic, GRP group) {
    for(uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++) 
    {
        int16_t altered_voltages[CELL];   
        memcpy(altered_voltages, &optimal_voltages.ac_codes, CELL * sizeof(int16_t));
        for (int cell = 0; cell < CELL; cell++) {
            altered_voltages[cell] -= (rand() % 100); //fluctates cell voltages
        }
        switch (voltage_testing)
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
            print("Unknown voltage emulation state.\n");
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
        switch (temp_testing)
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
            print("Unknown temperature emulation state.\n");
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
            printf("Invalid group access in read_aux.\n");
            exit(EXIT_FAILURE);
        }    
    }
}




