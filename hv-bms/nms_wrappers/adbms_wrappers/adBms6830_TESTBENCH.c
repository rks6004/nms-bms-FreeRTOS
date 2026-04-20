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
        memcpy(&altered_voltages, &optimal_voltages.ac_codes, CELL * sizeof(int16_t));
        for (int cell = 0; cell < CELL; cell++) {
            altered_voltages[cell] -= (rand() % 100); //fluctates cell voltages
        }
        switch (voltage_testing)
        {
        case VOLT_NORMAL:
            //only need to fuzz voltage values slightly for normal voltage, no further action needed   
            break;
        case VOLT_OV_BRIEF:
            // Handle overvoltage brief test
            break;
        case VOLT_UV_BRIEF:
            // Handle undervoltage brief test
            break;
        case VOLT_OV_CONSISTENT:
            // Handle overvoltage consistent test
            break;
        case VOLT_UV_CONSISTENT:
            // Handle undervoltage consistent test
            break;
        default:
            // Handle unknown state
            break;
        }
        if (group != ALL_GRP && group != F) {
            memcpy(&(ic[curr_ic].acell.ac_codes[(group - 1) * 3]), altered_voltages, (size_t)(3 * sizeof(int16_t)));
        }
        else if (group == F) {
            memcpy(&(ic[curr_ic].acell.ac_codes[(group - 1) * 3]), altered_voltages, (size_t)(sizeof(int16_t))); //only one register in group here
        }
        else { //ALLGRP
            memcpy(&(ic[curr_ic].acell.ac_codes), altered_voltages, (size_t)(CELL * sizeof(int16_t)));
        }    
    }
}




