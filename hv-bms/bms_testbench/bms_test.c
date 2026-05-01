#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"
#include "util.h"

/* Local includes. */
#include "console.h"

#include "bms_test.h"
#include "bms_util.h"
#include "main.h"

/*  BMS Includes */
bms_test_setup test_setup;

emulated_adbms_6830 characteristic_6830[ADBMS_6830_IC_NUM];
emulated_adbms_2950 characteristic_2950[ADBMS_2950_IC_NUM];

extern EventGroupHandle_t charging_evt_id;

EventGroupHandle_t testbench_evt_id;

extern SemaphoreHandle_t ioMutexHandle;

void get_test_filename(char* datapoint, char* full_filename) {
    char* mode;
    if (!strcasecmp(datapoint, "voltage")) {mode = test_setup.voltage_testing > VOLT_NORMAL ? "uv" : "normal";}
    else if (!strcasecmp(datapoint, "current")) {mode = test_setup.current_testing > CURR_NORMAL ? "oc" : "normal";}
    else if (!strcasecmp(datapoint, "temp")) {mode = test_setup.temp_testing ? "ot" : "normal";}
    else {return NULL;}
    sprintf(full_filename, sizeof(full_filename), "%s/%s_discharge_%s.csv\0", TEST_DATA_DIR, datapoint, mode);
}

void testbench_init(void) {
    #if CHARGING_TEST
        test_setup.charging_testing = true;
    #else
        test_setup.charging_testing = false;
    #endif

    #if VOLTAGE_TEST
        test_setup.voltage_testing = test_setup.charging_testing ? VOLT_OV : VOLT_UV;
    #else
        test_setup.voltage_testing = VOLT_NORMAL;
    #endif

    #if CURRENT_TEST
        test_setup.current_testing = CURR_OC;
    #else
        test_setup.current_testing = CURR_NORMAL;
    #endif

    #if SIGNAL_TEST == 3 //PEC_HEAVY_INTERFERENCE
        test_setup.pec_testing = PEC_HEAVY_INTERFERENCE;
    #elif SIGNAL_TEST == 2 //PEC_SLIGHT_INTERFERENCE
        test_setup.pec_testing = PEC_SLIGHT_INTERFERENCE;
    #else
        test_setup.pec_testing = PEC_NORMAL;
    #endif

    testbench_evt_id = xEventGroupCreate();
    xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
    printf("==================================\nSummary of Discharge Testing Criteria (as built):\n==================================\n");
    printf("Temperature Testing: %d\n", test_setup.temp_testing);
    printf("Current Testing: %d\n", test_setup.current_testing);
    printf("Voltage Testing: %d\n", test_setup.voltage_testing);
    printf("Signal Integrity Testing: %d\n", test_setup.pec_testing);
    printf("===============KEY================\n                                                       \
            TEMP_NORMAL = 1, TEMP_OT = 2\n");
    printf("CURR_NORMAL = 1, CURR_OC = 2\n");
    printf("VOLT_NORMAL = 1, VOLT_OV = 2, VOLT_UV = 3\n");
    printf("PEC_NORMAL = 1, PEC_SLIGHT_INTERFERENCE = 2, PEC_HEAVY_INTERFERENCE = 3\n                    \
            ==================================\n");
    xSemaphoreGive(ioMutexHandle);
    
    char curr_filename[64]; //length should be long enough to handle reasonably-sized tracefile names
    get_test_filename("current", curr_filename);
    
    char voltage_filename[64];
    get_test_filename("voltage", voltage_filename);
    
    char temp_filename[64];
    get_test_filename("temp", temp_filename);

    //PEC errors generated randomly, no need to grab files

    testbench_datastream* current_datastream = malloc((size_t)(MAX_SIZE_PER_DATASTREAM_LINE * (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION)));
    get_vehicle_testbench_data(curr_filename, current_datastream);

    testbench_datastream* voltage_datastream = malloc((size_t)(MAX_SIZE_PER_DATASTREAM_LINE * (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION)));
    get_vehicle_testbench_data(voltage_filename, voltage_datastream);

    testbench_datastream* temp_datastream = malloc((size_t)(MAX_SIZE_PER_DATASTREAM_LINE * (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION)));
    get_vehicle_testbench_data(temp_filename, temp_datastream);

    //as of right now, writing the same traces to all ADBMS chips, can revise in future to target specific segments if needed
    //may also need to create seperate testbench_datastream instances for each IC?
    for (int ic = 0; ic < ADBMS_2950_IC_NUM; ic++) {
        characteristic_2950[ic].current_data = current_datastream;
        characteristic_2950[ic].signal_behavior = test_setup.pec_testing;
    }
    for (int ic = 0; ic < ADBMS_6830_IC_NUM; ic++) {
        characteristic_6830[ic].temp_data = temp_datastream;
        characteristic_6830[ic].voltage_data = voltage_datastream;
    }
    return;
}

void testbench_task(void* argument) {
    testbench_init();
    xEventGroupSetBits(charging_evt_id, EVENT_FLAG_CHARGING_DISABLE); //DISCHARGING
    xTaskCreate(current_monitor,
            "currentMonitorTask",
            2048,
            NULL,
            PRIORITY_LOW,
            NULL);
    xTaskCreate(voltage_monitor,
            "voltageMonitorTask",
            2048,
            NULL,
            PRIORITY_LOW,
            NULL);
    xTaskCreate(temp_monitor,
            "tempMonitoringTask",
            2048,
            NULL,
            PRIORITY_LOW,
            NULL);
    xTaskCreate(signal_integrity_monitor,
            "signalIntegrityMonitorTask",
            2048,
            NULL,
            PRIORITY_LOW,
            NULL);
}

void current_monitor(void) {
    EventBits_t current_error_bits = 
    xEventGroupWaitBits(testbench_evt_id, 
        CURRENT_ERROR_BITS, 
        pdFALSE, 
        pdFALSE,
        pdMS_TO_TICKS(EVENT_TIMEOUT)
    );
    if (current_error_bits & CURRENT_ERROR_BITS) {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup.current_testing > CURR_NORMAL ? "Current Test: OC detected for overcurrent trace, PASS.\n" : "Current Test: OC detected for normal trace, FAIL.\n";
        printf("%s", status);
        xSemaphoreGive(ioMutexHandle);
    }
    vTaskDelete(NULL);
}

void voltage_monitor(void) {
    EventBits_t voltage_error_bits = 
    xEventGroupWaitBits(testbench_evt_id, 
        VOLTAGE_ERROR_BITS, 
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(EVENT_TIMEOUT)
    );
    if (voltage_error_bits & VOLTAGE_ERROR_BITS) {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup.voltage_testing > VOLT_NORMAL ? "Voltage Test: OV/UV detected for voltage fault trace, PASS.\n" : "Voltage Test: OV/UV detected for normal trace, FAIL.\n";
        printf("%s", status);
        xSemaphoreGive(ioMutexHandle);
    }
    vTaskDelete(NULL);
}

void temp_monitor(void) {
    EventBits_t temp_error_bits = 
    xEventGroupWaitBits(testbench_evt_id, 
        TEMP_ERROR_BITS, 
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(EVENT_TIMEOUT)
    );
    if (temp_error_bits & TEMP_ERROR_BITS) {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup.temp_testing > TEMP_NORMAL ? "Temperature Test: OT detected for temperature fault trace, PASS.\n" : "Temperature Test: OT detected for normal trace, FAIL.\n";
        printf("%s", status);
        xSemaphoreGive(ioMutexHandle);
    }
    vTaskDelete(NULL);
}

void signal_integrity_monitor(void) {
    EventBits_t signal_error_bits = 
    xEventGroupWaitBits(testbench_evt_id, 
        PEC_ERROR_BITS, 
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(EVENT_TIMEOUT)
    );
    if (signal_error_bits & PEC_ERROR_BITS) {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup.pec_testing > PEC_NORMAL ? "Signal Integrity Test: PEC error detected for interference trace, PASS.\n" : "Signal Integrity Test: PEC error detected for normal trace, FAIL.\n";
        printf("%s", status);
        xSemaphoreGive(ioMutexHandle);
    }
    vTaskDelete(NULL);
}

int get_vehicle_testbench_data(char* filepath, testbench_datastream* datastream){
    xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
    FILE* fptr;
    fptr = fopen(filepath, "r");
    if (fptr != NULL) {
        char line[MAX_SIZE_PER_DATASTREAM_LINE + 2]; //to accomodate newline and null terminator
        char* ret = fgets(line, MAX_SIZE_PER_DATASTREAM_LINE + 2, fptr); //should terminate 
        uint16_t counter = 0;
        while (ret != NULL && *ret != EOF) {
            char* field = strtok(line, ","); //gets timestamp
            if (field != NULL) {
                datastream->timestamps[counter] = (uint32_t)(atoi(field)); //assuming this will be unsigned anyways due to timestamp var being non-negative
            }
            field = strtok(NULL, ","); //gets value field
            if (field != NULL) {
                int received_value = atoi(field); 
                datastream->values[counter] = (int32_t)(received_value); 
            }
            counter++;
            ret = fgets(line, MAX_SIZE_PER_DATASTREAM_LINE + 2, fptr);
        }
    }
    else {
        printf("Could not obtain vehicle data teststream from provided filepath.\n");
        return EXIT_FAILURE;
    }
    xSemaphoreGive(ioMutexHandle);
    return EXIT_SUCCESS;
}
