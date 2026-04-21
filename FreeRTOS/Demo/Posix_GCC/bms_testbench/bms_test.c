#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"
#include "stdlib.h"
#include "bms_test.h"
#include "main.h"

/*  BMS Includes */
VOLTAGE_STATE voltage_testing;
TEMP_STATE temp_testing;
CURRENT_STATE current_testing;
PEC_STATE pec_testing;

emulated_adbms_6830 characteristic_6830[ADBMS_6830_IC_NUM];
emulated_adbms_2950 characteristic_2950[ADBMS_2950_IC_NUM];

void testbench_init(void) {
    switch (BMS_TEST_TYPE)
    {
    case NORMAL_DISCHARGE:
        voltage_testing = VOLT_NORMAL;
        temp_testing = TEMP_NORMAL;
        current_testing = CURR_NORMAL;
        pec_testing = PEC_NORMAL;
        break;
    case LOW_SOC_DISCHARGE:
        voltage_testing = VOLT_UV;
        temp_testing = TEMP_NORMAL;
        current_testing = CURR_NORMAL;
        pec_testing = PEC_NORMAL;
        break;
    case CURRENT_SPIKE_DISCHARGE:
        voltage_testing = VOLT_NORMAL;
        temp_testing = TEMP_NORMAL;
        current_testing = CURR_OC;
        pec_testing = PEC_NORMAL;
        break;
    case OVERHEAT_DISCHARGE:
        voltage_testing = VOLT_NORMAL;
        temp_testing = TEMP_OT;
        current_testing = CURR_NORMAL;
        pec_testing = PEC_NORMAL;
        break;
    case NORMAL_CHARGE:
        voltage_testing = VOLT_NORMAL;
        temp_testing = TEMP_NORMAL;
        current_testing = CURR_NORMAL;
        pec_testing = PEC_NORMAL;
        break;
    case OVERHEAT_CHARGE:
        voltage_testing = VOLT_NORMAL;
        temp_testing = TEMP_OT;
        current_testing = CURR_NORMAL;
        pec_testing = PEC_NORMAL;
        break;
    case SIGNAL_INTERFERENCE:
        voltage_testing = VOLT_NORMAL;
        temp_testing = TEMP_OT;
        current_testing = CURR_NORMAL;
        pec_testing = PEC_HEAVY_INTERFERENCE;
    default:
        voltage_testing = VOLT_NORMAL;
        temp_testing = TEMP_NORMAL;
        current_testing = CURR_NORMAL;
        pec_testing = PEC_NORMAL;
        break;
    }
}

