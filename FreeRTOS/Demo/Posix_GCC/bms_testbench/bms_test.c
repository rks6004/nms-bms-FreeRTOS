#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

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

extern EventGroupHandle_t charging_evt_id;

void testbench_init(void) {
    return;
}

void testbench_task(void* argument) {
    testbench_init();
    #ifdef CHARGING_TEST
        xEventGroupSetBits(charging_evt_id, EVENT_FLAG_CHARGING_ENABLE);
    #else //DISCHARGING
        xEventGroupSetBits(charging_evt_id, EVENT_FLAG_CHARGING_DISABLE);
    #endif
    

}

