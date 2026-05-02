#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

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
bms_test_setup* test_setup;

emulated_adbms_6830 characteristic_6830[ADBMS_6830_IC_NUM];
emulated_adbms_2950 characteristic_2950[ADBMS_2950_IC_NUM];

extern EventGroupHandle_t charging_evt_id;

EventGroupHandle_t testbench_evt_id;
EventGroupHandle_t testbench_datastream_ready;

EventBits_t expected_test_bits = 1 << (NUM_TEST_DIMS + 1);


extern SemaphoreHandle_t ioMutexHandle;

void get_test_filename(char* datapoint, char* full_filename) {
    char* mode;
    if (!strcasecmp(datapoint, "voltage")) {mode = test_setup->voltage_testing > VOLT_NORMAL ? "uv" : "normal";}
    else if (!strcasecmp(datapoint, "current")) {mode = test_setup->current_testing > CURR_NORMAL ? "oc" : "normal";}
    else if (!strcasecmp(datapoint, "temp")) {mode = test_setup->temp_testing ? "ot" : "normal";}
    else {
        full_filename = NULL; 
        return;
    }
    sprintf(full_filename, "%s/%s_discharge_%s.csv", TEST_DATA_DIR, datapoint, mode);
}

void testbench_init(void) {
    ioMutexHandle = xSemaphoreCreateMutex();

    test_setup = pvPortMalloc(sizeof(bms_test_setup));
    #if CHARGING_TEST
        test_setup->charging_testing = true;
    #else
        test_setup->charging_testing = false;
    #endif

    #if VOLTAGE_TEST
        test_setup->voltage_testing = VOLTAGE_TEST;
        expected_test_bits |= VOLTAGE_ERROR_BITS;
    #else
        test_setup->voltage_testing = VOLT_NORMAL;
    #endif

    #if CURRENT_TEST
        test_setup->current_testing = CURRENT_TEST;
        expected_test_bits |= CURRENT_ERROR_BITS;
    #else
        test_setup->current_testing = CURR_NORMAL;
    #endif

    #if TEMP_TEST
        test_setup->temp_testing = TEMP_TEST;
        expected_test_bits |= TEMP_ERROR_BITS;
    #else
        test_setup->temp_testing = TEMP_NORMAL;
    #endif

    #if SIGNAL_TEST
        test_setup->pec_testing = SIGNAL_TEST;
        expected_test_bits |= PEC_ERROR_BITS;
    #else
        test_setup->pec_testing = PEC_NORMAL;
    #endif

    xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
    printf("==================================\nSummary of Discharge Testing Criteria (as built):\n==================================\n");
    printf("Temperature Testing: %d\n", test_setup->temp_testing);
    printf("Current Testing: %d\n", test_setup->current_testing);
    printf("Voltage Testing: %d\n", test_setup->voltage_testing);
    printf("Signal Integrity Testing: %d\n", test_setup->pec_testing);
    printf("===============KEY================\nTEMP_NORMAL = 1, TEMP_OT = 2\n");
    printf("CURR_NORMAL = 1, CURR_OC = 2\n");
    printf("VOLT_NORMAL = 1, VOLT_OV = 2, VOLT_UV = 3\n");
    printf("PEC_NORMAL = 1, PEC_SLIGHT_INTERFERENCE = 2, PEC_HEAVY_INTERFERENCE = 3\n==================================\n");    
    xSemaphoreGive(ioMutexHandle);

    
    
    char curr_filename[64]; //length should be long enough to handle reasonably-sized tracefile names
    get_test_filename("current", curr_filename);
    
    char voltage_filename[64];
    get_test_filename("voltage", voltage_filename);

    
    char temp_filename[64];
    get_test_filename("temp", temp_filename);

    //PEC errors generated randomly, no need to grab files

    testbench_datastream* current_datastream = pvPortMalloc(sizeof(testbench_datastream));
    current_datastream->timestamps = pvPortMalloc((size_t)(sizeof(uint32_t) * (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION)));
    current_datastream->values = pvPortMalloc((size_t)(sizeof(int32_t) * (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION)));
    get_vehicle_testbench_data(curr_filename, current_datastream);

    testbench_datastream* voltage_datastream = pvPortMalloc(sizeof(testbench_datastream));
    voltage_datastream->timestamps = pvPortMalloc((size_t)(sizeof(uint32_t) * (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION)));
    voltage_datastream->values = pvPortMalloc((size_t)(sizeof(int32_t) * (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION))); 
    get_vehicle_testbench_data(voltage_filename, voltage_datastream);

    testbench_datastream* temp_datastream = pvPortMalloc(sizeof(testbench_datastream));
    temp_datastream->timestamps = pvPortMalloc((size_t)(sizeof(uint32_t) * (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION)));
    temp_datastream->values = pvPortMalloc((size_t)(sizeof(int32_t) * (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION))); 
    get_vehicle_testbench_data(temp_filename, temp_datastream);

    //as of right now, writing the same traces to all ADBMS chips, can revise in future to target specific segments if needed
    //may also need to create seperate testbench_datastream instances for each IC?
    for (int ic = 0; ic < ADBMS_2950_IC_NUM; ic++) {
        characteristic_2950[ic].current_data = current_datastream;
        characteristic_2950[ic].signal_behavior = test_setup->pec_testing;
    }
    for (int ic = 0; ic < ADBMS_6830_IC_NUM; ic++) {
        characteristic_6830[ic].temp_data = temp_datastream;
        characteristic_6830[ic].voltage_data = voltage_datastream;
        characteristic_6830[ic].signal_behavior = test_setup->pec_testing;
    }
    xEventGroupSetBits(testbench_datastream_ready, TESTBENCH_DATASTREAMS_READY_BITS);
    return;
}

void current_monitor(void) {
    EventBits_t current_error_bits = 
    xEventGroupWaitBits(testbench_evt_id, 
        CURRENT_ERROR_BITS, 
        pdFALSE, 
        pdFALSE,
        pdMS_TO_TICKS(EVENT_TIMEOUT)
    );
    printf("Done waiting in current_monitor.\n");
    if (current_error_bits & CURRENT_ERROR_BITS) {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup->current_testing > CURR_NORMAL ? "Current Test: OC detected for overcurrent trace, PASS.\n" : "Current Test: OC detected for normal trace, FAIL.\n";
        printf("%s", status);
        xSemaphoreGive(ioMutexHandle);
    } else {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup->current_testing > CURR_NORMAL ? "Current Test: No OC detected for overcurrent trace, FAIL.\n" : "Current Test: No OC detected for normal trace, PASS.\n";
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
        printf("Done waiting in voltage_monitor.\n");
    if (voltage_error_bits & VOLTAGE_ERROR_BITS) {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup->voltage_testing > VOLT_NORMAL ? "Voltage Test: OV/UV detected for voltage fault trace, PASS.\n" : "Voltage Test: OV/UV detected for normal trace, FAIL.\n";
        printf("%s", status);
        xSemaphoreGive(ioMutexHandle);
    } else {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup->voltage_testing > VOLT_NORMAL ? "Voltage Test: No OV/UV detected for voltage fault trace, FAIL.\n" : "Voltage Test: No OV/UV detected for normal trace, PASS.\n";
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
        printf("Done waiting in temp_monitor.\n");

    if (temp_error_bits & TEMP_ERROR_BITS) {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup->temp_testing > TEMP_NORMAL ? "Temperature Test: OT detected for temperature fault trace, PASS.\n" : "Temperature Test: OT detected for normal trace, FAIL.\n";
        printf("%s", status);
        xSemaphoreGive(ioMutexHandle);
    } else {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup->temp_testing > TEMP_NORMAL ? "Temperature Test: No OT detected for temperature fault trace, FAIL.\n" : "Temperature Test: No OT detected for normal trace, PASS.\n";
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
        printf("Done waiting in PEC_monitor\n.");

    if (signal_error_bits & PEC_ERROR_BITS) {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup->pec_testing > PEC_NORMAL ? "Signal Integrity Test: PEC error detected for interference trace, PASS.\n" : "Signal Integrity Test: PEC error detected for normal trace, FAIL.\n";
        printf("%s", status);
        xSemaphoreGive(ioMutexHandle);
    } else {
        xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
        char* status = test_setup->pec_testing > PEC_NORMAL ? "Signal Integrity Test: No PEC error detected for interference trace, FAIL.\n" : "Signal Integrity Test: No PEC error detected for normal trace, PASS.\n";
        printf("%s", status);
        xSemaphoreGive(ioMutexHandle);
    }
    vTaskDelete(NULL);
}

int get_vehicle_testbench_data(char* filepath, testbench_datastream* datastream){
    xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
    // printf("File being parsed: %s\n", filepath);
    FILE* fptr;
    fptr = fopen(filepath, "r");
    if (fptr != NULL && datastream != NULL) {
        char line[MAX_SIZE_PER_DATASTREAM_LINE];
        uint16_t counter = 0;
        while (fgets(line, MAX_SIZE_PER_DATASTREAM_LINE, fptr) != NULL) {
            char* timestamp_field = strtok(line, ",");
            char* value_field = strtok(NULL, ",\n\r"); // strip carriage return and newline
            // printf("Parsed: %s, %s\n", timestamp_field, value_field);
            if (timestamp_field != NULL && value_field != NULL) {
                // Bounds check to prevent buffer overflow
                if (counter >= (TEST_STREAM_MAX_LENGTH_MS / TEST_STREAM_TIMING_RESOLUTION)) {
                    break;
                }
                datastream->timestamps[counter] = (uint32_t)(atoi(timestamp_field));
                datastream->values[counter] = (int32_t)(atoi(value_field));
                counter++;
            }
            
        }
        fclose(fptr);
    }
    else {
        printf("Could not obtain vehicle data teststream from provided filepath.\n");
        return EXIT_FAILURE;
    }
    xSemaphoreGive(ioMutexHandle);
    return EXIT_SUCCESS;
}

void testbench_exit() {
    // Free current_datastream (all ICs reference the same pointer)
    if (characteristic_2950[0].current_data != NULL) {
        testbench_datastream* current_data = characteristic_2950[0].current_data;
        if (current_data->timestamps != NULL) {
            vPortFree(current_data->timestamps);
        }
        if (current_data->values != NULL) {
            vPortFree(current_data->values);
        }
        vPortFree(current_data);
        
        // Null out all references
        for (int ic = 0; ic < ADBMS_2950_IC_NUM; ic++) {
            characteristic_2950[ic].current_data = NULL;
        }
    }
    // Free voltage_datastream (all ICs reference the same pointer)
    if (characteristic_6830[0].voltage_data != NULL) {
        testbench_datastream* voltage_data = characteristic_6830[0].voltage_data;
        if (voltage_data->timestamps != NULL) {
            vPortFree(voltage_data->timestamps);
        }
        if (voltage_data->values != NULL) {
            vPortFree(voltage_data->values);
        }
        vPortFree(voltage_data);
        
        // Null out all references
        for (int ic = 0; ic < ADBMS_6830_IC_NUM; ic++) {
            characteristic_6830[ic].voltage_data = NULL;
        }
    }
    // Free temp_datastream (all ICs reference the same pointer)
    if (characteristic_6830[0].temp_data != NULL) {
        testbench_datastream* temp_data = characteristic_6830[0].temp_data;
        if (temp_data->timestamps != NULL) {
            vPortFree(temp_data->timestamps);
        }
        if (temp_data->values != NULL) {
            vPortFree(temp_data->values);
        }
        vPortFree(temp_data);
        
        // Null out all references
        for (int ic = 0; ic < ADBMS_6830_IC_NUM; ic++) {
            characteristic_6830[ic].temp_data = NULL;
        }
    }
}

void testbench_task(void* argument) {
    xEventGroupSetBits(charging_evt_id, EVENT_FLAG_CHARGING_DISABLE); //DISCHARGING
    xTaskCreate((TaskFunction_t)current_monitor,
            "currentMonitorTask",
            2048,
            NULL,
            PRIORITY_LOW,
            NULL);
    xTaskCreate((TaskFunction_t)voltage_monitor,
            "voltageMonitorTask",
            2048,
            NULL,
            PRIORITY_LOW,
            NULL);
    xTaskCreate((TaskFunction_t)temp_monitor,
            "tempMonitoringTask",
            2048,
            NULL,
            PRIORITY_LOW,
            NULL);
    xTaskCreate((TaskFunction_t)signal_integrity_monitor,
            "signalIntegrityMonitorTask",
            2048,
            NULL,
            PRIORITY_LOW,
            NULL);
    vTaskDelay(pdMS_TO_TICKS(TEST_STREAM_MAX_LENGTH_MS)); //allows for all traces to conclude to make sure all errors were flagged
    xEventGroupSetBits(testbench_evt_id, 1 << (NUM_TEST_DIMS + 1));
    testbench_exit();
    vTaskDelete(NULL);
}
