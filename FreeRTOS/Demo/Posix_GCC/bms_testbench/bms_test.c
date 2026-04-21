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

/*  BMS Includes */
extern int test_stream_read(void* filepath);

extern VOLTAGE_STATE voltage_testing;
extern TEMP_STATE temp_testing;
extern CURRENT_STATE current_testing;
extern PEC_STATE pec_testing; 

int bms_file_reading_test(void) {
    console_print("Starting the BMS test in specific code.\n");
    int ret = test_stream_read("./read_test");
    return ret;
}

int bms_task_tester(void* data) {
    console_print("%d\n", *(int*)data);
    return 0;
}

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

